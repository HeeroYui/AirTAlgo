/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <airtalgo/Resampler.h>
#include <iostream>

#undef __class__
#define __class__ "Resampler"

airtalgo::Resampler::Resampler() :
  #ifdef HAVE_SPEEX_DSP_RESAMPLE
    m_speexResampler(nullptr),
  #endif
  m_positionRead(0),
  m_positionWrite(0) {
	
}

void airtalgo::Resampler::init() {
	airtalgo::Algo::init();
}

std::shared_ptr<airtalgo::Resampler> airtalgo::Resampler::create() {
	std::shared_ptr<airtalgo::Resampler> tmp(new airtalgo::Resampler());
	tmp->init();
	return tmp;
}

airtalgo::Resampler::~Resampler() {
	#ifdef HAVE_SPEEX_DSP_RESAMPLE
		if (m_speexResampler != nullptr) {
			speex_resampler_destroy(m_speexResampler);
			m_speexResampler = nullptr;
		}
	#endif
}

void airtalgo::Resampler::configurationChange() {
	airtalgo::Algo::configurationChange();
	if (m_input.getFormat() != m_output.getFormat()) {
		AIRTALGO_ERROR("can not support Format Change ...");
		m_needProcess = false;
	}
	if (m_input.getFormat() != format_int16) {
		AIRTALGO_ERROR("can not support Format other than int16_t ...");
		m_needProcess = false;
		return;
	}
	if (m_output.getMap() != m_output.getMap()) {
		AIRTALGO_ERROR("can not support map Change ...");
		m_needProcess = false;
	}
	if (m_input.getFrequency() == m_output.getFrequency()) {
		// nothing to process...
		m_needProcess = false;
		return;
	}
	#ifdef HAVE_SPEEX_DSP_RESAMPLE
		if (m_speexResampler != nullptr) {
			speex_resampler_destroy(m_speexResampler);
			m_speexResampler = nullptr;
		}
		int err = 0;
		m_speexResampler = speex_resampler_init(m_output.getMap().size(),
		                                        m_input.getFrequency(),
		                                        m_output.getFrequency(),
		                                        10, &err);
	#else
		AIRTALGO_WARNING("SPEEX DSP lib not accessible ==> can not resample");
		m_needProcess = false;
	#endif
}


bool airtalgo::Resampler::process(std::chrono::system_clock::time_point& _time,
                                  void* _input,
                                  size_t _inputNbChunk,
                                  void*& _output,
                                  size_t& _outputNbChunk) {
	airtalgo::autoLogInOut tmpLog("Resampler");
	_outputNbChunk = 2048;
	// chack if we need to process:
	if (m_needProcess == false) {
		AIRTALGO_WARNING("no process");
		_output = _input;
		_outputNbChunk = _inputNbChunk;
		return true;
	}
	if (_input == nullptr) {
		_output = &(m_outputData[0]);
		_outputNbChunk = 0;
		AIRTALGO_ERROR("null pointer input ... ");
		return false;
	}
	#ifdef HAVE_SPEEX_DSP_RESAMPLE
		float nbInputTime = float(_inputNbChunk)/m_input.getFrequency();
		float nbOutputSample = nbInputTime*m_output.getFrequency();
		// we add 10% of the buffer size to have all the time enought data in the output to proceed all the input data...
		_outputNbChunk = size_t(nbOutputSample*1.5f);
		AIRTALGO_VERBOSE("                               freq in=" << m_input.getFrequency() << " out=" << m_output.getFrequency());
		AIRTALGO_VERBOSE("                               Frame duration=" << nbInputTime);
		AIRTALGO_VERBOSE("                               nbInput chunk=" << _inputNbChunk << " nbOutputChunk=" << nbOutputSample);
		
		m_outputData.resize(_outputNbChunk*m_output.getMap().size()*m_formatSize*16);
		_output = &(m_outputData[0]);
		if (m_speexResampler == nullptr) {
			AIRTALGO_ERROR("                               No speex resampler");
			return false;
		}
		uint32_t nbChunkInput = _inputNbChunk;
		uint32_t nbChunkOutput = _outputNbChunk;
		AIRTALGO_VERBOSE("                               >> input=" << nbChunkInput << " output=" << nbChunkOutput);
		int ret = speex_resampler_process_interleaved_int(m_speexResampler,
		                                                  static_cast<int16_t*>(_input),
		                                                  &nbChunkInput,
		                                                  static_cast<int16_t*>(_output),
		                                                  &nbChunkOutput);
		AIRTALGO_VERBOSE("                               << input=" << nbChunkInput << " output=" << nbChunkOutput);
		// update position of data:
		m_positionWrite += nbChunkOutput;
		// Check all input and output ...
		if (nbChunkInput != _inputNbChunk) {
			AIRTALGO_ERROR("                               inputSize (not all read ...) proceed=" << nbChunkInput << " requested=" << _inputNbChunk);
			// TODO : manage this case ...
		}
		if (nbChunkOutput == _outputNbChunk) {
			AIRTALGO_ERROR("                               Might have not enought data in output... output size=" << _outputNbChunk);
			// TODO : manage this case ...
		}
		_outputNbChunk = nbChunkOutput;
		AIRTALGO_VERBOSE("                               process chunk=" << nbChunkInput << " out=" << nbChunkOutput);
		return true;
	#else
		_output = _input;
		_outputNbChunk = _inputNbChunk;
		return false;
	#endif
}
