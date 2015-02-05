/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "IOFormatInterface.h"
#include "debug.h"

#undef __class__
#define __class__ "IOFormatInterface"


std::ostream& drain::operator <<(std::ostream& _os, const IOFormatInterface& _obj) {
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

drain::IOFormatInterface::IOFormatInterface() :
  m_configured(false),
  m_format(audio::format_unknow),
  m_map(),
  m_frequency(0) {
	m_map.push_back(audio::channel_frontLeft);
	m_map.push_back(audio::channel_frontRight);
}

drain::IOFormatInterface::IOFormatInterface(std::vector<audio::channel> _map, audio::format _format, float _frequency) :
  m_configured(true),
  m_format(_format),
  m_map(_map),
  m_frequency(_frequency) {
	
}

void drain::IOFormatInterface::set(std::vector<audio::channel> _map, audio::format _format, float _frequency) {
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

void drain::IOFormatInterface::setConfigured(bool _value) {
	m_configured = _value;
}

bool drain::IOFormatInterface::getConfigured() const {
	return m_configured;
}

audio::format drain::IOFormatInterface::getFormat() const {
	return m_format;
}

void drain::IOFormatInterface::setFormat(audio::format _value) {
	if (m_format == _value) {
		return;
	}
	m_format = _value;
	m_configured = true;
	configurationChange();
}

const std::vector<audio::channel>& drain::IOFormatInterface::getMap() const{
	return m_map;
}

void drain::IOFormatInterface::setMap(const std::vector<audio::channel>& _value) {
	if (m_map == _value) {
		return;
	}
	m_map = _value;
	m_configured = true;
	configurationChange();
}

float drain::IOFormatInterface::getFrequency() const{
	return m_frequency;
}

void drain::IOFormatInterface::setFrequency(float _value) {
	if (m_frequency == _value) {
		return;
	}
	m_configured = true;
	m_frequency = _value;
	configurationChange();
}

void drain::IOFormatInterface::configurationChange() {
	if (m_ioChangeFunctor != nullptr) {
		m_ioChangeFunctor();
	}
}

void drain::IOFormatInterface::setCallback(const std::function<void()>& _functor) {
	m_ioChangeFunctor = _functor;
}
