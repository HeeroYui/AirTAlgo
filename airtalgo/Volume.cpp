/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <airtalgo/Volume.h>
#include <iostream>

#undef __class__
#define __class__ "Volume"

airtalgo::Volume::Volume() :
  m_volumeAppli(1.0f),
  m_functionConvert(nullptr) {
	
}

void airtalgo::Volume::init() {
	airtalgo::Algo::init();
	m_type = "Volume";
	m_supportedFormat.push_back(audio::format_int16);
	m_supportedFormat.push_back(audio::format_int16_on_int32);
}

std::shared_ptr<airtalgo::Volume> airtalgo::Volume::create() {
	std::shared_ptr<airtalgo::Volume> tmp(new airtalgo::Volume());
	tmp->init();
	return tmp;
}

airtalgo::Volume::~Volume() {
	
}

static int32_t neareastsss(float _val) {
	int32_t out = 0;
	while (_val > float(1<<out)) {
		out++;
	}
	return std::min(16,out);
}


static void convert__int16__to__int16(void* _input, void* _output, size_t _nbSample, int32_t _volumeCoef, int32_t _volumeDecalage, float _volumeAppli) {
	int16_t* in = static_cast<int16_t*>(_input);
	int16_t* out = static_cast<int16_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		out[iii] = int16_t((int32_t(in[iii]) * int32_t(_volumeCoef)) >> _volumeDecalage);
	}
}

static void convert__int16__to__int32(void* _input, void* _output, size_t _nbSample, int32_t _volumeCoef, int32_t _volumeDecalage, float _volumeAppli) {
	int16_t* in = static_cast<int16_t*>(_input);
	int32_t* out = static_cast<int32_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		out[iii] = (int32_t(in[iii]) * int32_t(_volumeCoef)) >> _volumeDecalage;
	}
	//AIRTALGO_INFO("plop " << in[0] << " >> " << out[0]);
}

static void convert__int32__to__int16(void* _input, void* _output, size_t _nbSample, int32_t _volumeCoef, int32_t _volumeDecalage, float _volumeAppli) {
	int32_t* in = static_cast<int32_t*>(_input);
	int16_t* out = static_cast<int16_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		int32_t value = in[iii] >> 16;
		out[iii] = int16_t((int64_t(in[iii]) * int64_t(_volumeCoef)) >> _volumeDecalage);
	}
}

static void convert__int32__to__int32(void* _input, void* _output, size_t _nbSample, int32_t _volumeCoef, int32_t _volumeDecalage, float _volumeAppli) {
	int32_t* in = static_cast<int32_t*>(_input);
	int32_t* out = static_cast<int32_t*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		out[iii] = int32_t((int64_t(in[iii]) * int64_t(_volumeCoef)) >> _volumeDecalage);
	}
	//AIRTALGO_INFO("plop " << in[0] << " >> " << out[0]);
}

static void convert__float__to__float(void* _input, void* _output, size_t _nbSample, int32_t _volumeCoef, int32_t _volumeDecalage, float _volumeAppli) {
	float* in = static_cast<float*>(_input);
	float* out = static_cast<float*>(_output);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		out[iii] = in[iii] * _volumeAppli;
	}
}

void airtalgo::Volume::configurationChange() {
	airtalgo::Algo::configurationChange();
	switch (m_input.getFormat()) {
		default:
		case audio::format_int16:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					m_functionConvert = &convert__int16__to__int16;
					AIRTALGO_DEBUG("Use converter : 'convert__int16__to__int16' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_int16_on_int32:
				case audio::format_int32:
					m_functionConvert = &convert__int16__to__int32;
					AIRTALGO_DEBUG("Use converter : 'convert__int16__to__int32' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_float:
					AIRTALGO_ERROR("Impossible case 1");
					break;
			}
			break;
		case audio::format_int16_on_int32:
		case audio::format_int32:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					m_functionConvert = &convert__int32__to__int16;
					AIRTALGO_DEBUG("Use converter : 'convert__int32__to__int16' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_int16_on_int32:
				case audio::format_int32:
					m_functionConvert = &convert__int32__to__int32;
					AIRTALGO_DEBUG("Use converter : 'convert__int32__to__int32' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
				case audio::format_float:
					AIRTALGO_ERROR("Impossible case 2");
					break;
			}
			break;
		case audio::format_float:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
				case audio::format_int16_on_int32:
				case audio::format_int32:
					AIRTALGO_ERROR("Impossible case 4");
					break;
				case audio::format_float:
					m_functionConvert = &convert__float__to__float;
					AIRTALGO_DEBUG("Use converter : 'convert__float__to__float' for " << m_input.getFormat() << " to " << m_output.getFormat());
					break;
			}
			break;
	}
	if (m_input.getMap() != m_output.getMap()) {
		AIRTALGO_ERROR("Volume map change is not supported");
	}
	if (m_input.getFrequency() != m_output.getFrequency()) {
		AIRTALGO_ERROR("Volume frequency change is not supported");
	}
	// nee to process all time (the format not change (just a simple filter))
	m_needProcess = true;
	volumeChange();
}

void airtalgo::Volume::volumeChange() {
	//m_volumeAppli = 20 * log(m_volumedB);
	float volumedB = 0.0f;
	for (auto &it : m_volumeList) {
		if (it == nullptr) {
			continue;
		}
		volumedB += it->getVolume();
		AIRTALGO_VERBOSE("append volume : '" << it->getName() << " vol=" << it->getVolume() << "dB");
	}
	AIRTALGO_DEBUG(" Total volume : " << volumedB << "dB nbVolume=" << m_volumeList.size());
	m_volumeAppli = std::pow(10.0f, volumedB/20.0f);
	switch (m_input.getFormat()) {
		default:
		case audio::format_int16:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					if (m_volumeAppli <= 1.0f) {
						m_volumeCoef = m_volumeAppli*float(1<<16);
						m_volumeDecalage = 16;
					} else {
						int32_t neareast = neareastsss(m_volumeAppli);
						m_volumeCoef = m_volumeAppli*float(1<<(16-neareast));
						m_volumeDecalage = 16-neareast;
					}
					break;
				case audio::format_int16_on_int32:
					if (m_volumeAppli <= 1.0f) {
						m_volumeCoef = m_volumeAppli*float(1<<16);
						m_volumeDecalage = 16;
					} else {
						int32_t neareast = neareastsss(m_volumeAppli);
						m_volumeCoef = m_volumeAppli*float(1<<(16-neareast));
						m_volumeDecalage = 16-neareast;
					}
					break;
				case audio::format_int32:
					m_volumeCoef = m_volumeAppli*float(1<<16);
					m_volumeDecalage = 0;
					break;
				case audio::format_float:
					AIRTALGO_ERROR("Impossible case 1");
					break;
			}
			break;
		case audio::format_int16_on_int32:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					if (m_volumeAppli <= 1.0f) {
						m_volumeCoef = m_volumeAppli*float(1<<16);
						m_volumeDecalage = 16;
					} else {
						int32_t neareast = neareastsss(m_volumeAppli);
						m_volumeCoef = m_volumeAppli*float(1<<(16-neareast));
						m_volumeDecalage = 16-neareast;
					}
					break;
				case audio::format_int16_on_int32:
					if (m_volumeAppli <= 1.0f) {
						m_volumeCoef = m_volumeAppli*float(1<<16);
						m_volumeDecalage = 16;
					} else {
						int32_t neareast = neareastsss(m_volumeAppli);
						m_volumeCoef = m_volumeAppli*float(1<<(16-neareast));
						m_volumeDecalage = 16-neareast;
					}
					break;
				case audio::format_int32:
					m_volumeCoef = m_volumeAppli*float(1<<16);
					m_volumeDecalage = 0;
					break;
				case audio::format_float:
					AIRTALGO_ERROR("Impossible case 2");
					break;
			}
			break;
		case audio::format_int32:
			switch (m_output.getFormat()) {
				default:
				case audio::format_int16:
					if (m_volumeAppli <= 1.0f) {
						m_volumeCoef = m_volumeAppli*float(1<<16);
						m_volumeDecalage = 32;
					} else {
						int32_t neareast = neareastsss(m_volumeAppli);
						m_volumeCoef = m_volumeAppli*float(1<<(16-neareast));
						m_volumeDecalage = 32-neareast;
					}
					break;
				case audio::format_int16_on_int32:
					if (m_volumeAppli <= 1.0f) {
						m_volumeCoef = m_volumeAppli*float(1<<16);
						m_volumeDecalage = 32;
					} else {
						int32_t neareast = neareastsss(m_volumeAppli);
						m_volumeCoef = m_volumeAppli*float(1<<(16-neareast));
						m_volumeDecalage = 32-neareast;
					}
					break;
				case audio::format_int32:
					m_volumeCoef = m_volumeAppli*float(1<<16);
					m_volumeDecalage = 16;
					break;
				case audio::format_float:
					AIRTALGO_ERROR("Impossible case 3");
					break;
			}
			break;
		case audio::format_float:
			// nothing to do (use m_volumeAppli)
			break;
	}
}


std::vector<audio::format> airtalgo::Volume::getFormatSupportedInput() {
	std::vector<audio::format> tmp;
	if (m_output.getFormat() == audio::format_float) {
		tmp.push_back(audio::format_float);
	}
	if (    m_output.getFormat() == audio::format_int16
	     || m_output.getFormat() == audio::format_int16_on_int32
	     || m_output.getFormat() == audio::format_int32) {
		tmp.push_back(audio::format_int16);
		tmp.push_back(audio::format_int16_on_int32);
		tmp.push_back(audio::format_int32);
	}
	return tmp;
};

std::vector<audio::format> airtalgo::Volume::getFormatSupportedOutput() {
		std::vector<audio::format> tmp;
	if (m_input.getFormat() == audio::format_float) {
		tmp.push_back(audio::format_float);
	}
	if (    m_input.getFormat() == audio::format_int16
	     || m_input.getFormat() == audio::format_int16_on_int32
	     || m_input.getFormat() == audio::format_int32) {
		tmp.push_back(audio::format_int16);
		tmp.push_back(audio::format_int16_on_int32);
		tmp.push_back(audio::format_int32);
	}
	return tmp;
};


bool airtalgo::Volume::process(std::chrono::system_clock::time_point& _time,
                               void* _input,
                               size_t _inputNbChunk,
                               void*& _output,
                               size_t& _outputNbChunk) {
	airtalgo::AutoLogInOut tmpLog("Volume");
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
	//AIRTALGO_WARNING("Apply volume : " << m_volumedB << "dB " << m_volumeAppli << " ==> x*" << m_volumeCoef << ">>" << m_volumeDecalage << " ex:50*C>>D=" << (50*m_volumeCoef>>m_volumeDecalage) );
	m_functionConvert(_input, _output, _outputNbChunk*m_input.getMap().size(), m_volumeCoef, m_volumeDecalage, m_volumeAppli);
	return true;
}

void airtalgo::Volume::addVolumeStage(const std::shared_ptr<VolumeElement>& _volume) {
	if (_volume == nullptr) {
		return;
	}
	for (auto &it : m_volumeList) {
		if (it == nullptr) {
			continue;
		}
		if (it == _volume) {
			// already done ...
			return;
		}
		if (it->getName() == _volume->getName()) {
			return;
		}
	}
	m_volumeList.push_back(_volume);
	volumeChange();
}

bool airtalgo::Volume::setParameter(const std::string& _parameter, const std::string& _value) {
	if (_parameter == "FLOW") {
		// set Volume ...
		for (auto &it : m_volumeList) {
			if (it == nullptr) {
				continue;
			}
			if (it->getName() == "FLOW") {
				float value = 0;
				if (sscanf(_value.c_str(), "%fdB", &value) != 1) {
					return false;
				}
				if (    value < -300
				     || value > 300) {
					AIRTALGO_ERROR("Can not set volume ... : '" << _parameter << "' out of range : [-300..300]");
					return false;
				}
				it->setVolume(value);
				AIRTALGO_DEBUG("Set volume : FLOW = " << value << " dB (from:" << _value << ")");
				volumeChange();
				return true;
			}
		}
	}
	AIRTALGO_ERROR("unknow set Parameter : '" << _parameter << "' with Value: '" << _value << "'");
	return false;
}

std::string airtalgo::Volume::getParameter(const std::string& _parameter) const {
	if (_parameter == "FLOW") {
		// set Volume ...
		for (auto &it : m_volumeList) {
			if (it == nullptr) {
				continue;
			}
			if (it->getName() == "FLOW") {
				return std::to_string(it->getVolume()) + "dB";
			}
		}
	}
	AIRTALGO_ERROR("unknow get Parameter : '" << _parameter << "'");
	return "[ERROR]";
}

std::string airtalgo::Volume::getParameterProperty(const std::string& _parameter) const {
	if (_parameter == "FLOW") {
		// set Volume ...
		for (auto &it : m_volumeList) {
			if (it == nullptr) {
				continue;
			}
			if (it->getName() == "FLOW") {
				return "[-300..300]dB";
			}
		}
	}
	AIRTALGO_ERROR("unknow Parameter property for: '" << _parameter << "'");
	return "[ERROR]";
}

