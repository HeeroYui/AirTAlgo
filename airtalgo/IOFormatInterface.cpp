/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "IOFormatInterface.h"
#include "debug.h"

#undef __class__
#define __class__ "IOFormatInterface"


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

airtalgo::IOFormatInterface::IOFormatInterface() :
  m_configured(false),
  m_format(audio::format_unknow),
  m_map(),
  m_frequency(0) {
	m_map.push_back(audio::channel_frontLeft);
	m_map.push_back(audio::channel_frontRight);
}

airtalgo::IOFormatInterface::IOFormatInterface(std::vector<audio::channel> _map, audio::format _format, float _frequency) :
  m_configured(true),
  m_format(_format),
  m_map(_map),
  m_frequency(_frequency) {
	
}

void airtalgo::IOFormatInterface::set(std::vector<audio::channel> _map, audio::format _format, float _frequency) {
	bool hasChange = false;
	if (m_map != _map) {
		m_map = _map;
		hasChange = true;
	}
	if (m_format != _format) {
		m_format = _format;
		hasChange = true;
	}
	if (m_frequency != _frequency) {
		m_frequency = _frequency;
		hasChange = true;
	}
	if (hasChange == true) {
		m_configured = true;
		configurationChange();
	}
}

void airtalgo::IOFormatInterface::setConfigured(bool _value) {
	m_configured = _value;
}

bool airtalgo::IOFormatInterface::getConfigured() const {
	return m_configured;
}

audio::format airtalgo::IOFormatInterface::getFormat() const {
	return m_format;
}

void airtalgo::IOFormatInterface::setFormat(audio::format _value) {
	if (m_format == _value) {
		return;
	}
	m_format = _value;
	m_configured = true;
	configurationChange();
}

const std::vector<audio::channel>& airtalgo::IOFormatInterface::getMap() const{
	return m_map;
}

void airtalgo::IOFormatInterface::setMap(const std::vector<audio::channel>& _value) {
	if (m_map == _value) {
		return;
	}
	m_map = _value;
	m_configured = true;
	configurationChange();
}

float airtalgo::IOFormatInterface::getFrequency() const{
	return m_frequency;
}

void airtalgo::IOFormatInterface::setFrequency(float _value) {
	if (m_frequency == _value) {
		return;
	}
	m_configured = true;
	m_frequency = _value;
	configurationChange();
}

void airtalgo::IOFormatInterface::configurationChange() {
	if (m_ioChangeFunctor != nullptr) {
		m_ioChangeFunctor();
	}
}

void airtalgo::IOFormatInterface::setCallback(const std::function<void()>& _functor) {
	m_ioChangeFunctor = _functor;
}
