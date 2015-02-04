/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <airtalgo/FormatUpdate.h>
#include <iostream>

#undef __class__
#define __class__ "FormatUpdate"

#ifndef INT16_MAX
	#define INT16_MAX 0x7fff
#endif
#ifndef INT16_MIN
	#define INT16_MIN (-INT16_MAX - 1)
#endif
#ifndef INT32_MAX
	#define INT32_MAX 0x7fffffffL
#endif
#ifndef INT32_MIN
	#define INT32_MIN (-INT32_MAX - 1L)
#endif


static void convert__int16__to__int16_on_int32(void* _input, void* _output, size_t _nbSample) {
	int16_t* in = static_cast<int16_t*>(_input);
	int32_t* out = static_cast<int32_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		out[iii] = in[iii];
	}
}
static void convert__int16__to__int32(void* _input, void* _output, size_t _nbSample) {
	int16_t* in = static_cast<int16_t*>(_input);
	int32_t* out = static_cast<int32_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		out[iii] = in[iii]<<16;
	}
}
static void convert__int16__to__float(void* _input, void* _output, size_t _nbSample) {
	int16_t* in = static_cast<int16_t*>(_input);
	float* out = static_cast<float*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		// TODO : Set * intead of /
		out[iii] = static_cast<float>(in[iii])/static_cast<float>(INT16_MAX);
	}
}

static void convert__int16_on_int32__to__int16(void* _input, void* _output, size_t _nbSample) {
	int32_t* in = static_cast<int32_t*>(_input);
	int16_t* out = static_cast<int16_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		out[iii] = static_cast<int16_t>(std::min(std::max(INT16_MIN, in[iii]), INT16_MAX));
	}
}
static void convert__int16_on_int32__to__int32(void* _input, void* _output, size_t _nbSample) {
	int32_t* in = static_cast<int32_t*>(_input);
	int32_t* out = static_cast<int32_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		int32_t value = std::min(std::max(INT16_MIN, in[iii]), INT16_MAX);
		out[iii] = value << 16;
	}
}
static void convert__int16_on_int32__to__float(void* _input, void* _output, size_t _nbSample) {
	int32_t* in = static_cast<int32_t*>(_input);
	float* out = static_cast<float*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		// TODO : Set * intead of /
		out[iii] = static_cast<float>(in[iii])/static_cast<float>(INT16_MAX);
	}
}

static void convert__int32__to__int16(void* _input, void* _output, size_t _nbSample) {
	int32_t* in = static_cast<int32_t*>(_input);
	int16_t* out = static_cast<int16_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		int32_t value = in[iii] >> 16;
		out[iii] = static_cast<int16_t>(std::min(std::max(INT16_MIN, value), INT16_MAX));
	}
}
static void convert__int32__to__int16_on_int32(void* _input, void* _output, size_t _nbSample) {
	int32_t* in = static_cast<int32_t*>(_input);
	int16_t* out = static_cast<int16_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		out[iii] = in[iii] >> 16;
	}
}
static void convert__int32__to__float(void* _input, void* _output, size_t _nbSample) {
	int32_t* in = static_cast<int32_t*>(_input);
	float* out = static_cast<float*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		// TODO : Set * intead of /
		out[iii] = static_cast<float>(in[iii])/static_cast<float>(INT32_MAX);
	}
}

static void convert__float__to__int16(void* _input, void* _output, size_t _nbSample) {
	float* in = static_cast<float*>(_input);
	int16_t* out = static_cast<int16_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		float value = in[iii] * static_cast<float>(INT16_MAX);
		value = std::min(std::max(static_cast<float>(INT16_MIN), value), static_cast<float>(INT16_MAX));
		out[iii] = static_cast<int16_t>(value);
		//AIRTALGO_DEBUG(iii << " in=" << in[iii] << " out=" << out[iii]);
	}
}
static void convert__float__to__int16_on_int32(void* _input, void* _output, size_t _nbSample) {
	float* in = static_cast<float*>(_input);
	int32_t* out = static_cast<int32_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		float value = in[iii] * static_cast<float>(INT16_MAX);
		value = std::min(std::max(static_cast<float>(INT32_MIN), value), static_cast<float>(INT32_MAX));
		out[iii] = static_cast<int32_t>(value);
	}
}
static void convert__float__to__int32(void* _input, void* _output, size_t _nbSample) {
	float* in = static_cast<float*>(_input);
	int32_t* out = static_cast<int32_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		float value = in[iii] * static_cast<float>(INT32_MAX);
		value = std::min(std::max(static_cast<float>(INT32_MIN), value), static_cast<float>(INT32_MAX));
		out[iii] = static_cast<int32_t>(value);
	}
}


airtalgo::FormatUpdate::FormatUpdate() :
  m_functionConvert(nullptr) {
	
}

void airtalgo::FormatUpdate::init() {
	airtalgo::Algo::init();
	m_type = "FormatUpdate";
}

std::shared_ptr<airtalgo::FormatUpdate> airtalgo::FormatUpdate::create() {
	std::shared_ptr<airtalgo::FormatUpdate> tmp(new airtalgo::FormatUpdate());
	tmp->init();
	return tmp;
}

void airtalgo::FormatUpdate::configurationChange() {
	airtalgo::Algo::configurationChange();
	if (m_input.getMap() != m_output.getMap()) {
		AIRTALGO_ERROR("can not support Map Change ...");
		m_needProcess = false;
	}
	if (m_input.getFrequency() != m_output.getFrequency()) {
		AIRTALGO_ERROR("can not support frequency Change ...");
		m_needProcess = false;
	}
	if (m_input.getFormat() == m_output.getFormat()) {
		// nothing to process...
		m_needProcess = false;
		return;
	}
	switch (m_input.getFormat()) {
		default:
		case audio::format_int16:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					AIRTALGO_ERROR(" Impossible case 1");
					break;
				case audio::format_int16_on_int32:
					m_functionConvert = &convert__int16__to__int16_on_int32;
					AIRTALGO_DEBUG(" use converter : 'convert__int16__to__int16_on_int32' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_int32:
					m_functionConvert = &convert__int16__to__int32;
					AIRTALGO_DEBUG(" use converter : 'convert__int16__to__int32' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_float:
					m_functionConvert = &convert__int16__to__float;
					AIRTALGO_DEBUG(" use converter : 'convert__int16__to__float' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
			}
			break;
		case audio::format_int16_on_int32:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					m_functionConvert = &convert__int16_on_int32__to__int16;
					AIRTALGO_DEBUG(" use converter : 'convert__int16_on_int32__to__int16' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_int16_on_int32:
					AIRTALGO_ERROR(" Impossible case 2");
					break;
				case audio::format_int32:
					m_functionConvert = &convert__int16_on_int32__to__int32;
					AIRTALGO_DEBUG(" use converter : 'convert__int16_on_int32__to__int32' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_float:
					m_functionConvert = &convert__int16_on_int32__to__float;
					AIRTALGO_DEBUG(" use converter : 'convert__int16_on_int32__to__float' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
			}
			break;
		case audio::format_int32:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					m_functionConvert = &convert__int32__to__int16;
					AIRTALGO_DEBUG(" use converter : 'convert__int32__to__int16' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_int16_on_int32:
					m_functionConvert = &convert__int32__to__int16_on_int32;
					AIRTALGO_DEBUG(" use converter : 'convert__int32__to__int16_on_int32' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_int32:
					AIRTALGO_ERROR(" Impossible case 3");
					break;
				case audio::format_float:
					m_functionConvert = &convert__int32__to__float;
					AIRTALGO_DEBUG(" use converter : 'convert__int32__to__float' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
			}
			break;
		case audio::format_float:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					m_functionConvert = &convert__float__to__int16;
					AIRTALGO_DEBUG(" use converter : 'convert__float__to__int16' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_int16_on_int32:
					m_functionConvert = &convert__float__to__int16_on_int32;
					AIRTALGO_DEBUG(" use converter : 'convert__float__to__int16_on_int32' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_int32:
					m_functionConvert = &convert__float__to__int32;
					AIRTALGO_DEBUG(" use converter : 'convert__float__to__int32' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_float:
					AIRTALGO_ERROR(" Impossible case 4");
					break;
			}
			break;
	}
}


bool airtalgo::FormatUpdate::process(std::chrono::system_clock::time_point& _time,
                                            void* _input,
                                            size_t _inputNbChunk,
                                            void*& _output,
                                            size_t& _outputNbChunk) {
	airtalgo::AutoLogInOut tmpLog("FormatUpdate");
	// chack if we need to process:
	if (m_needProcess == false) {
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
	_outputNbChunk = _inputNbChunk;
	m_outputData.resize(_outputNbChunk*m_input.getMap().size()*m_formatSize);
	_output = &(m_outputData[0]);
	if (m_functionConvert == nullptr) {
		AIRTALGO_ERROR("null function ptr");
		return false;
	}
	m_functionConvert(_input, _output, _outputNbChunk*m_input.getMap().size());
	return true;
}
