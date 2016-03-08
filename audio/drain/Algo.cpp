/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <audio/drain/Algo.h>
#include <functional>
#include "debug.h"


#undef __class__
#define __class__ "Algo"

audio::drain::Algo::Algo() :
  m_temporary(false),
  m_outputData(),
  m_formatSize(0),
  m_needProcess(false) {
	
}

void audio::drain::Algo::init() {
	// set notification callback :
	m_input.setCallback(std::bind(&audio::drain::Algo::configurationChangeLocal, this));
	m_output.setCallback(std::bind(&audio::drain::Algo::configurationChangeLocal, this));
	// first configure ==> update the internal parameters
	configurationChange();
}

void audio::drain::Algo::configurationChange() {
	m_needProcess = false;
	if (m_input.getFormat() != m_output.getFormat()) {
		m_needProcess = true;
	}
	if (m_input.getMap() != m_output.getMap()) {
		m_needProcess = true;
	}
	if (m_input.getFrequency() != m_output.getFrequency()) {
		m_needProcess = true;
	}
	switch (m_output.getFormat()) {
		case audio::format_int8:
			m_formatSize = sizeof(int8_t);
			break;
		case audio::format_int16:
			m_formatSize = sizeof(int16_t);
			break;
		case audio::format_int24:
		case audio::format_int16_on_int32:
		case audio::format_int32:
			m_formatSize = sizeof(int32_t);
			break;
		case audio::format_float:
			m_formatSize = sizeof(float);
			break;
		case audio::format_double:
			m_formatSize = sizeof(double);
			break;
		case audio::format_unknow:
			DRAIN_VERBOSE("format not configured...");
			m_formatSize = 8;
			break;
	}
}

size_t audio::drain::Algo::needInputData(size_t _output) {
	size_t input = _output;
	/* NOT good at all ...
	if (m_input.getFormat() != m_output.getFormat()) {
		int32_t inputSize = 3;
		switch (m_input.getFormat()) {
			case format_int16:
				inputSize = sizeof(int16_t);
				break;
			case format_int16_on_int32:
			case format_int32:
				inputSize = sizeof(int32_t);
				break;
			case format_float:
				inputSize = sizeof(float);
				break;
		}
		if (inputSize != m_formatSize) {
			input *= inputSize;
			input /= m_formatSize;
		}
	}
	*/
	/*
	if (m_input.getMap().size() != m_output.getMap().size()) {
		input *= m_input.getMap().size();
		input /= m_output.getMap().size();
	}
	*/
	
	if (m_input.getFrequency() != m_output.getFrequency()) {
		input *= m_input.getFrequency();
		input /= m_output.getFrequency();
		// to prevent missing data in the resampler
		input += 2;
	}
	return input;
}

void audio::drain::Algo::setStatusFunction(algoStatusFunction _newFunction) {
	m_statusFunction = _newFunction;
}

void audio::drain::Algo::generateStatus(const std::string& _status) {
	if (m_statusFunction != nullptr) {
		if (m_name.size() == 0) {
			m_statusFunction(m_type, _status);
		} else {
			m_statusFunction(m_name, _status);
		}
	}
}