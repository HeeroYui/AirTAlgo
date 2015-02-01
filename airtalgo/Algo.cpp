/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <airtalgo/Algo.h>
#include <functional>
#include "debug.h"


std::ostream& airtalgo::operator <<(std::ostream& _os, const IOFormatInterface& _obj) {
	_os << "{";
	if (_obj.getConfigured() == false) {
		_os << "Not configured";
	} else {
		_os << "format=" << _obj.getFormat();
		_os << ", frequency=" << _obj.getFrequency();
		_os << ", map=" << _obj.getMap();
	}
	_os << "}";
	return _os;
}

#undef __class__
#define __class__ "Algo"

airtalgo::Algo::Algo() :
  m_temporary(false),
  m_outputData(),
  m_formatSize(0),
  m_needProcess(false) {
	AIRTALGO_VERBOSE("CREATE ALGO");
}

void airtalgo::Algo::init() {
	// set notification callback :
	m_input.setCallback(std::bind(&airtalgo::Algo::configurationChangeLocal, this));
	m_output.setCallback(std::bind(&airtalgo::Algo::configurationChangeLocal, this));
	// first configure ==> update the internal parameters
	configurationChange();
}

void airtalgo::Algo::configurationChange() {
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
		case format_int16:
			m_formatSize = sizeof(int16_t);
			break;
		case format_int16_on_int32:
		case format_int32:
			m_formatSize = sizeof(int32_t);
			break;
		case format_float:
			m_formatSize = sizeof(float);
			break;
		case format_unknow:
			AIRTALGO_ERROR("format not configured...");
			m_formatSize = 8;
			break;
	}
}

size_t airtalgo::Algo::needInputData(size_t _output) {
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
