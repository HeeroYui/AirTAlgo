/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <airtalgo/EndPointCallback.h>

#undef __class__
#define __class__ "EndPointCallback"

airtalgo::EndPointCallback::EndPointCallback() :
  m_outputFunction(nullptr),
  m_inputFunction(nullptr) {
	
}

void airtalgo::EndPointCallback::init(needDataFunction _callback) {
	m_outputFunction = _callback;
	airtalgo::EndPoint::init();
	m_type = "EndPointCallback";
}
void airtalgo::EndPointCallback::init(haveNewDataFunction _callback) {
	m_inputFunction = _callback;
	airtalgo::EndPoint::init();
	m_type = "EndPointCallback";
}

std::shared_ptr<airtalgo::EndPointCallback> airtalgo::EndPointCallback::create(needDataFunction _callback) {
	std::shared_ptr<airtalgo::EndPointCallback> tmp(new airtalgo::EndPointCallback());
	tmp->init(_callback);
	return tmp;
}

std::shared_ptr<airtalgo::EndPointCallback> airtalgo::EndPointCallback::create(haveNewDataFunction _callback) {
	std::shared_ptr<airtalgo::EndPointCallback> tmp(new airtalgo::EndPointCallback());
	tmp->init(_callback);
	return tmp;
}

void airtalgo::EndPointCallback::configurationChange() {
	airtalgo::EndPoint::configurationChange();
	AIRTALGO_INFO("update : format=" << m_output.getFormat() << " map=" << m_input.getMap() << " freq=" << m_input.getFrequency() << " size=" << int32_t(m_formatSize));
	m_needProcess = true;
}


bool airtalgo::EndPointCallback::process(std::chrono::system_clock::time_point& _time,
                                         void* _input, // uneeded
                                         size_t _inputNbChunk, // requested number of sample ...
                                         void*& _output,
                                         size_t& _outputNbChunk){
	airtalgo::autoLogInOut tmpLog("EndPointCallback");
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

