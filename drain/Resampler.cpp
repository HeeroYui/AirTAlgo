/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <drain/Resampler.h>
#include <iostream>

#undef __class__
#define __class__ "Resampler"

drain::Resampler::Resampler() :
  #ifdef HAVE_SPEEX_DSP_RESAMPLE
    m_speexResampler(nullptr),
  #endif
  m_positionRead(0),
  m_positionWrite(0) {
	
}

void drain::Resampler::init() {
	drain::Algo::init();
	m_type = "Resampler";
	m_supportedFormat.push_back(audio::format_int16);
	m_residualTimeInResampler = std11::chrono::nanoseconds(0);
}

std11::shared_ptr<drain::Resampler> drain::Resampler::create() {
	std11::shared_ptr<drain::Resampler> tmp(new drain::Resampler());
	tmp->init();
	return tmp;
}

drain::Resampler::~Resampler() {
	#ifdef HAVE_SPEEX_DSP_RESAMPLE
		if (m_speexResampler != nullptr) {
			speex_resampler_destroy(m_speexResampler);
			m_speexResampler = nullptr;
		}
	#endif
}

void drain::Resampler::configurationChange() {
	drain::Algo::configurationChange();
	if (m_input.getFormat() != m_output.getFormat()) {
		DRAIN_ERROR("can not support Format Change ...");
		m_needProcess = false;
	}
	if (m_input.getFormat() != audio::format_int16) {
		DRAIN_ERROR("can not support Format other than int16_t ...");
		m_needProcess = false;
		return;
	}
	if (m_output.getMap() != m_output.getMap()) {
		DRAIN_ERROR("can not support map Change ...");
		m_needProcess = false;
	}
	if (m_input.getFrequency() == m_output.getFrequency()) {
		// nothing to process...
		m_needProcess = false;
		return;
	}
	if (    m_input.getFrequency() == 0
	     || m_output.getFrequency() == 0) {
		DRAIN_WARNING("Configure IO with 0 frequency ... " << m_input << " to " << m_output);
		return;
	}
	#ifdef HAVE_SPEEX_DSP_RESAMPLE
		if (m_speexResampler != nullptr) {
			speex_resampler_destroy(m_speexResampler);
			m_speexResampler = nullptr;
		}
		int err = 0;
		DRAIN_WARNING("Create resampler for : " << m_input << " to " << m_output);
		m_speexResampler = speex_resampler_init(m_output.getMap().size(),
		                                        m_input.getFrequency(),
		                                        m_output.getFrequency(),
		                                        10, &err);
		m_residualTimeInResampler = std11::chrono::nanoseconds(0);
	#else
		DRAIN_WARNING("SPEEX DSP lib not accessible ==> can not resample");
		m_needProcess = false;
	#endif
}

bool drain::Resampler::process(std11::chrono::system_clock::time_point& _time,
                               void* _input,
                               size_t _inputNbChunk,
                               void*& _output,
                               size_t& _outputNbChunk) {
	drain::AutoLogInOut tmpLog("Resampler");
	_outputNbChunk = 2048;
	// chack if we need to process:
	if (m_needProcess == false) {
		DRAIN_WARNING("no process");
		_output = _input;
		_outputNbChunk = _inputNbChunk;
		return true;
	}
	if (_input == nullptr) {
		_output = &(m_outputData[0]);
		_outputNbChunk = 0;
		DRAIN_ERROR("null pointer input ... ");
		return false;
	}
	// Update Output time with the previous delta of the buffer
	DRAIN_VERBOSE("Resampler correct timestamp : " << _time << " ==> " << (_time - m_residualTimeInResampler));
	_time -= m_residualTimeInResampler;
	
	std11::chrono::nanoseconds inTime((int64_t(_inputNbChunk)*1000000000LL) / int64_t(m_input.getFrequency()));
	m_residualTimeInResampler += inTime;
	#ifdef HAVE_SPEEX_DSP_RESAMPLE
		float nbInputTime = float(_inputNbChunk)/m_input.getFrequency();
		float nbOutputSample = nbInputTime*m_output.getFrequency();
		// we add 10% of the buffer size to have all the time enought data in the output to proceed all the input data...
		_outputNbChunk = size_t(nbOutputSample*1.5f);
		DRAIN_VERBOSE("                               freq in=" << m_input.getFrequency() << " out=" << m_output.getFrequency());
		DRAIN_VERBOSE("                               Frame duration=" << nbInputTime);
		DRAIN_VERBOSE("                               nbInput chunk=" << _inputNbChunk << " nbOutputChunk=" << nbOutputSample);
		
		m_outputData.resize(_outputNbChunk*m_output.getMap().size()*m_formatSize*16);
		_output = &(m_outputData[0]);
		if (m_speexResampler == nullptr) {
			DRAIN_ERROR("                               No speex resampler");
			return false;
		}
		uint32_t nbChunkInput = _inputNbChunk;
		uint32_t nbChunkOutput = _outputNbChunk;
		DRAIN_VERBOSE("                               >> input=" << nbChunkInput << " output=" << nbChunkOutput);
		int ret = speex_resampler_process_interleaved_int(m_speexResampler,
		                                                  static_cast<int16_t*>(_input),
		                                                  &nbChunkInput,
		                                                  static_cast<int16_t*>(_output),
		                                                  &nbChunkOutput);
		DRAIN_VERBOSE("                               << input=" << nbChunkInput << " output=" << nbChunkOutput);
		// update position of data:
		m_positionWrite += nbChunkOutput;
		// Check all input and output ...
		if (nbChunkInput != _inputNbChunk) {
			DRAIN_ERROR("                               inputSize (not all read ...) proceed=" << nbChunkInput << " requested=" << _inputNbChunk);
			// TODO : manage this case ...
		}
		if (nbChunkOutput == _outputNbChunk) {
			DRAIN_ERROR("                               Might have not enought data in output... output size=" << _outputNbChunk);
			// TODO : manage this case ...
		}
		_outputNbChunk = nbChunkOutput;
		DRAIN_VERBOSE("                               process chunk=" << nbChunkInput << " out=" << nbChunkOutput);
		std11::chrono::nanoseconds outTime((int64_t(_outputNbChunk)*1000000000LL) / int64_t(m_output.getFrequency()));
		DRAIN_VERBOSE("convert " << _inputNbChunk << " ==> " << _outputNbChunk << "    " << inTime.count() << " => " << outTime.count());
		// correct time :
		m_residualTimeInResampler -= outTime;
		/*
		if (m_residualTimeInResampler.count() < 0) {
			DRAIN_TODO("manage this case ... residual time in resampler : " << m_residualTimeInResampler.count() << "ns");
		}
		*/
		return true;
	#else
		_output = _input;
		_outputNbChunk = _inputNbChunk;
		return false;
	#endif
}
