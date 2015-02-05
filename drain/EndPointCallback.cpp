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

void drain::EndPointCallback::init(needDataFunction _callback) {
	m_outputFunction = _callback;
	drain::EndPoint::init();
	m_type = "EndPointCallback";
}
void drain::EndPointCallback::init(haveNewDataFunction _callback) {
	m_inputFunction = _callback;
	drain::EndPoint::init();
	m_type = "EndPointCallback";
}

std::shared_ptr<drain::EndPointCallback> drain::EndPointCallback::create(needDataFunction _callback) {
	std::shared_ptr<drain::EndPointCallback> tmp(new drain::EndPointCallback());
	tmp->init(_callback);
	return tmp;
}

std::shared_ptr<drain::EndPointCallback> drain::EndPointCallback::create(haveNewDataFunction _callback) {
	std::shared_ptr<drain::EndPointCallback> tmp(new drain::EndPointCallback());
	tmp->init(_callback);
	return tmp;
}

void drain::EndPointCallback::configurationChange() {
	drain::EndPoint::configurationChange();
	AIRTALGO_INFO("update : format=" << m_output.getFormat() << " map=" << m_input.getMap() << " freq=" << m_input.getFrequency() << " size=" << int32_t(m_formatSize));
	m_needProcess = true;
}


bool drain::EndPointCallback::process(std::chrono::system_clock::time_point& _time,
                                         void* _input, // uneeded
                                         size_t _inputNbChunk, // requested number of sample ...
                                         void*& _output,
                                         size_t& _outputNbChunk){
	drain::AutoLogInOut tmpLog("EndPointCallback");
	if (m_outputFunction != nullptr) {
		// update buffer size ...
		m_outputData.resize(_inputNbChunk*m_output.getMap().size()*m_formatSize);
		// call user
		AIRTALGO_VERBOSE("call user get " << _inputNbChunk << "*" << m_output.getMap().size() << " map=" << m_output.getMap() << " datasize=" << int32_t(m_formatSize));
		m_outputFunction(_time,
		                 _inputNbChunk,
		                 m_output.getMap(),
		                 &m_outputData[0],
		                 m_output.getFormat());
		if (m_outputData.size() != _inputNbChunk*m_output.getMap().size()*m_formatSize) {
			AIRTALGO_ERROR(" can not get enough data from user ... " << m_outputData.size() << " != " << _inputNbChunk*m_output.getMap().size());
			return false;
		}
		_output = &m_outputData[0];
		_outputNbChunk = _inputNbChunk;
		return true;
	}
	if (m_inputFunction != nullptr) {
		// Call user ...
		AIRTALGO_VERBOSE("call user set " << _inputNbChunk << "*" << m_input.getMap().size());
		m_inputFunction(_time,
		                _inputNbChunk,
		                m_input.getMap(),
		                _input,
		                m_input.getFormat());
		return true;
	}
	return false;
}

