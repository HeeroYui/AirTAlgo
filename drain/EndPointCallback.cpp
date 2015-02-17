/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <drain/EndPointCallback.h>

#undef __class__
#define __class__ "EndPointCallback"

drain::EndPointCallback::EndPointCallback() :
  m_outputFunction(nullptr),
  m_inputFunction(nullptr) {
	
}

void drain::EndPointCallback::init(playbackFunction _callback) {
	m_outputFunction = _callback;
	drain::EndPoint::init();
	m_type = "EndPointCallback";
}
void drain::EndPointCallback::init(recordFunction _callback) {
	m_inputFunction = _callback;
	drain::EndPoint::init();
	m_type = "EndPointCallback";
}

std::shared_ptr<drain::EndPointCallback> drain::EndPointCallback::create(playbackFunction _callback) {
	std::shared_ptr<drain::EndPointCallback> tmp(new drain::EndPointCallback());
	tmp->init(_callback);
	return tmp;
}

std::shared_ptr<drain::EndPointCallback> drain::EndPointCallback::create(recordFunction _callback) {
	std::shared_ptr<drain::EndPointCallback> tmp(new drain::EndPointCallback());
	tmp->init(_callback);
	return tmp;
}

void drain::EndPointCallback::configurationChange() {
	drain::EndPoint::configurationChange();
	DRAIN_INFO("update : format=" << m_output.getFormat() << " map=" << m_input.getMap() << " freq=" << m_input.getFrequency() << " size=" << int32_t(m_formatSize));
	m_needProcess = true;
}


bool drain::EndPointCallback::process(std::chrono::system_clock::time_point& _time,
                                      void* _input,
                                      size_t _inputNbChunk,
                                      void*& _output,
                                      size_t& _outputNbChunk){
	drain::AutoLogInOut tmpLog("EndPointCallback");
	if (m_inputFunction != nullptr) {
		// Call user ...
		DRAIN_VERBOSE("call user set " << _inputNbChunk << "*" << m_input.getMap().size());
		m_inputFunction(_input,
		                _time,
		                _inputNbChunk,
		                m_output.getFormat(),
		                m_output.getFrequency(),
		                m_output.getMap());
		return true;
	}
	if (m_outputFunction != nullptr) {
		// update buffer size ...
		m_outputData.resize(_inputNbChunk*m_output.getMap().size()*m_formatSize);
		// call user
		DRAIN_VERBOSE("call user get " << _inputNbChunk << "*" << m_output.getMap().size() << " map=" << m_output.getMap() << " datasize=" << int32_t(m_formatSize));
		m_outputFunction(&m_outputData[0],
		                 _time,
		                 _inputNbChunk,
		                 m_output.getFormat(),
		                 m_output.getFrequency(),
		                 m_output.getMap());
		if (m_outputData.size() != _inputNbChunk*m_output.getMap().size()*m_formatSize) {
			DRAIN_ERROR(" can not get enough data from user ... " << m_outputData.size() << " != " << _inputNbChunk*m_output.getMap().size());
			return false;
		}
		_output = &m_outputData[0];
		_outputNbChunk = _inputNbChunk;
		return true;
	}
	return false;
}

