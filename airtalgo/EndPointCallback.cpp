/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <airtalgo/EndPointCallback.h>

airtalgo::EndPointCallback::EndPointCallback(needDataFunction _callback, enum formatDataType _dataFormat) :
  m_dataFormat(_dataFormat),
  m_output(_callback),
  m_input(nullptr) {
	
}
airtalgo::EndPointCallback::EndPointCallback(haveNewDataFunction _callback, enum formatDataType _dataFormat) :
  m_dataFormat(_dataFormat),
  m_output(nullptr),
  m_input(_callback) {
	
}


void airtalgo::EndPointCallback::configurationChange() {
	airtalgo::EndPoint::configurationChange();
	m_needProcess = true;
}


bool airtalgo::EndPointCallback::process(std::chrono::system_clock::time_point& _time,
                                         void* _input, // uneeded
                                         size_t _inputNbChunk, // requested number of sample ...
                                         void*& _output,
                                         size_t& _outputNbChunk){
	airtalgo::autoLogInOut("EndPointCallback");
	/*
	if (m_output != nullptr) {
		// call user
		AIRTALGO_INFO("call user get I16*" << _inputNbChunk << "*" << airtalgo::Algo::m_output.getMap().size() << " " << airtalgo::Algo::m_output.getMap());
				m_data = m_output(_time, _inputNbChunk, airtalgo::Algo::m_output.getMap());
		if (m_data.size() != _inputNbChunk*airtalgo::Algo::m_output.getMap().size()) {
			//ERROR
			return false;
		}
		_output = &m_data[0];
		_outputNbChunk = _inputNbChunk;
		return true;
	}
	if (m_input != nullptr) {
		// convert in data :
		// TODO  : ...
		m_data.resize(_inputNbChunk*airtalgo::Algo::m_input.getMap().size());
		int16_t* data = static_cast<int16_t*>(_input);
		for (size_t iii; iii<m_data.size(); ++iii) {
			m_data[iii] = *data++;
		}
		// Call user ...
		AIRTALGO_INFO("call user set I16*" << _inputNbChunk << "*" << airtalgo::Algo::m_input.getMap().size());
		m_input(_time, _inputNbChunk, airtalgo::Algo::m_input.getMap(), m_data);
		return true;
	}
	*/
	return false;
}

