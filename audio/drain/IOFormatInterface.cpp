/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include "IOFormatInterface.hpp"
#include "debug.hpp"

etk::Stream& audio::drain::operator <<(etk::Stream& _os, const IOFormatInterface& _obj) {
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

audio::drain::IOFormatInterface::IOFormatInterface() :
  m_configured(false),
  m_format(audio::format_unknow),
  m_map(),
  m_frequency(0) {
	m_map.pushBack(audio::channel_frontLeft);
	m_map.pushBack(audio::channel_frontRight);
}

audio::drain::IOFormatInterface::IOFormatInterface(etk::Vector<enum audio::channel> _map, enum audio::format _format, float _frequency) :
  m_configured(true),
  m_format(_format),
  m_map(_map),
  m_frequency(_frequency) {
	
}

void audio::drain::IOFormatInterface::set(etk::Vector<enum audio::channel> _map, enum audio::format _format, float _frequency) {
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

void audio::drain::IOFormatInterface::setConfigured(bool _value) {
	m_configured = _value;
}

bool audio::drain::IOFormatInterface::getConfigured() const {
	return m_configured;
}

enum audio::format audio::drain::IOFormatInterface::getFormat() const {
	return m_format;
}

void audio::drain::IOFormatInterface::setFormat(enum audio::format _value) {
	if (m_format == _value) {
		return;
	}
	m_format = _value;
	m_configured = true;
	configurationChange();
}

const etk::Vector<enum audio::channel>& audio::drain::IOFormatInterface::getMap() const{
	return m_map;
}

void audio::drain::IOFormatInterface::setMap(const etk::Vector<enum audio::channel>& _value) {
	if (m_map == _value) {
		return;
	}
	m_map = _value;
	m_configured = true;
	configurationChange();
}

float audio::drain::IOFormatInterface::getFrequency() const{
	return m_frequency;
}

void audio::drain::IOFormatInterface::setFrequency(float _value) {
	if (m_frequency == _value) {
		return;
	}
	m_configured = true;
	m_frequency = _value;
	configurationChange();
}

void audio::drain::IOFormatInterface::configurationChange() {
	if (m_ioChangeFunctor != nullptr) {
		m_ioChangeFunctor();
	}
}
void audio::drain::IOFormatInterface::setCallback(const etk::Function<void()>& _functor) {
	m_ioChangeFunctor = _functor;
}


int32_t audio::drain::IOFormatInterface::getChunkSize() const {
	return m_map.size() * audio::getFormatBytes(m_format);
}
