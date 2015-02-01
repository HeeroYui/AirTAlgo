/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include "debug.h"
#include <airtalgo/EndPointWrite.h>


#undef __class__
#define __class__ "EndPointWrite"

airtalgo::EndPointWrite::EndPointWrite() :
  m_function(nullptr) {
	
}

void airtalgo::EndPointWrite::init() {
	airtalgo::EndPoint::init();
	m_type = "EndPoint";
}

std::shared_ptr<airtalgo::EndPointWrite> airtalgo::EndPointWrite::create() {
	std::shared_ptr<airtalgo::EndPointWrite> tmp(new airtalgo::EndPointWrite());
	tmp->init();
	return tmp;
}

void airtalgo::EndPointWrite::configurationChange() {
	airtalgo::EndPoint::configurationChange();
	m_needProcess = true;
}


bool airtalgo::EndPointWrite::process(std::chrono::system_clock::time_point& _time,
                                             void* _input,
                                             size_t _inputNbChunk,
                                             void*& _output,
                                             size_t& _outputNbChunk){
	airtalgo::autoLogInOut tmpLog("EndPointWrite");
	//AIRTALGO_INFO("                              nb Sample in buffer : " << m_tmpData.size());
	if (m_function != nullptr) {
		if (m_tmpData.size() <= 20000) {
			m_function(_time, _inputNbChunk, m_output.getMap(), m_output.getFormat());
		}
	}
	// resize output buffer:
	//AIRTALGO_INFO("    resize : " << (int32_t)m_formatSize << "*" << (int32_t)_inputNbChunk << "*" << (int32_t)m_outputMap.size());
	m_outputData.resize(m_formatSize*_inputNbChunk*m_output.getMap().size(), 0);
	// set output pointer:
	_outputNbChunk = m_outputData.size()/(m_formatSize*m_output.getMap().size());
	_output = &m_outputData[0];
	std::unique_lock<std::mutex> lock(m_mutex);
	// check if data in the tmpBuffer
	if (m_tmpData.size() == 0) {
		AIRTALGO_WARNING("No data in the user buffer (write null data ... " << _outputNbChunk << " chunks)");
		// just send no data ...
		return true;
	}
	AIRTALGO_INFO("Write " << _outputNbChunk << " chunks");
	// check if we have enought data:
	int32_t nbChunkToCopy = std::min(_inputNbChunk, m_tmpData.size()/(m_output.getMap().size()*m_formatSize));
	
	AIRTALGO_INFO("      " << nbChunkToCopy << " chunks ==> " << nbChunkToCopy*m_output.getMap().size()*m_formatSize << " Byte sizeBuffer=" << m_tmpData.size());
	// copy data to the output:
	memcpy(_output, &m_tmpData[0], nbChunkToCopy*m_output.getMap().size()*m_formatSize);
	// remove old data:
	m_tmpData.erase(m_tmpData.begin(), m_tmpData.begin() + nbChunkToCopy*m_output.getMap().size()*m_formatSize);
	//AIRTALGO_INFO("                              nb Sample in buffer : " << m_tmpData.size());
	return true;
}

void airtalgo::EndPointWrite::write(const void* _value, size_t _nbChunk) {
	std::unique_lock<std::mutex> lock(m_mutex);
	AIRTALGO_INFO("[ASYNC] Get data : " << _nbChunk << " chunks"
	              << " ==> " << _nbChunk*m_output.getMap().size() << " samples"
	              << " formatSize=" << int32_t(m_formatSize));
	const int8_t* value = static_cast<const int8_t*>(_value);
	for (size_t iii=0; iii<_nbChunk*m_formatSize*m_output.getMap().size(); ++iii) {
		m_tmpData.push_back(*value++);
	}
}
