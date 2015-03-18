/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include "debug.h"
#include <drain/EndPointWrite.h>


#undef __class__
#define __class__ "EndPointWrite"

drain::EndPointWrite::EndPointWrite() :
  m_function(nullptr),
  m_bufferSizeMicroseconds(1000000) {
	
}

void drain::EndPointWrite::init() {
	drain::EndPoint::init();
	m_type = "EndPoint";
	if (    audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size() != 0
	     && m_output.getFrequency() != 0) {
		m_buffer.setCapacity(m_bufferSizeMicroseconds,
		                     audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size(),
		                     m_output.getFrequency());
	}
}

std11::shared_ptr<drain::EndPointWrite> drain::EndPointWrite::create() {
	std11::shared_ptr<drain::EndPointWrite> tmp(new drain::EndPointWrite());
	tmp->init();
	return tmp;
}

void drain::EndPointWrite::configurationChange() {
	drain::EndPoint::configurationChange();
	// update the buffer size ...
	if (    audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size() != 0
	     && m_output.getFrequency() != 0) {
		if (std11::chrono::microseconds(0) != m_bufferSizeMicroseconds) {
			m_buffer.setCapacity(m_bufferSizeMicroseconds,
			                     audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size(),
			                     m_output.getFrequency());
		} else {
			m_buffer.setCapacity(m_bufferSizeChunk,
			                     audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size(),
			                     m_output.getFrequency());
		}
	}
	m_needProcess = true;
}


bool drain::EndPointWrite::process(std11::chrono::system_clock::time_point& _time,
                                   void* _input,
                                   size_t _inputNbChunk,
                                   void*& _output,
                                   size_t& _outputNbChunk){
	drain::AutoLogInOut tmpLog("EndPointWrite");
	//DRAIN_INFO("                              nb Sample in buffer : " << m_buffer.size());
	if (m_function != nullptr) {
		// TODO : Rework this ...
		if (m_buffer.getSize() <= 20000) {
			m_function(_time, _inputNbChunk, m_output.getFormat(), m_output.getFrequency(), m_output.getMap());
		}
	}
	// resize output buffer:
	//DRAIN_INFO("    resize : " << (int32_t)m_formatSize << "*" << (int32_t)_inputNbChunk << "*" << (int32_t)m_outputMap.size());
	m_outputData.resize(m_formatSize*_inputNbChunk*m_output.getMap().size());
	// set output pointer:
	_outputNbChunk = m_outputData.size()/(m_formatSize*m_output.getMap().size());
	_output = &m_outputData[0];
	std11::unique_lock<std11::mutex> lock(m_mutex);
	// check if data in the tmpBuffer
	if (m_buffer.getSize() == 0) {
		DRAIN_WARNING("No data in the user buffer (write null data ... " << _outputNbChunk << " chunks)");
		// clear the buffer to force the flush on the next elements ...
		m_outputData.clear();
		_outputNbChunk = 0;
		generateStatus("EPW_UNDERFLOW");
		// just send no data ...
		return true;
	}
	DRAIN_VERBOSE("Write " << _outputNbChunk << " chunks");
	// check if we have enought data:
	int32_t nbChunkToCopy = std::min(_inputNbChunk, m_buffer.getSize());
	if (nbChunkToCopy != _inputNbChunk) {
		generateStatus("EPW_UNDERFLOW");
	}
	DRAIN_VERBOSE("      " << nbChunkToCopy << " chunks ==> " << nbChunkToCopy*m_output.getMap().size()*m_formatSize << " Byte sizeBuffer=" << m_buffer.getSize());
	_outputNbChunk = nbChunkToCopy;
	// copy data to the output:
	int32_t nbUnderflow = m_buffer.read(_output, nbChunkToCopy);
	if (nbUnderflow != 0) {
		DRAIN_WARNING("Undeflow in FIFO ...");
		_outputNbChunk -= nbUnderflow;
	}
	return true;
}

void drain::EndPointWrite::write(const void* _value, size_t _nbChunk) {
	std11::unique_lock<std11::mutex> lock(m_mutex);
	DRAIN_VERBOSE("[ASYNC] Write data : " << _nbChunk << " chunks" << " ==> " << m_output);
	int32_t nbOverflow = m_buffer.write(_value, _nbChunk);
	if (nbOverflow > 0) {
		DRAIN_ERROR("Overflow in output buffer : " << nbOverflow << " / " << _nbChunk);
	}
}

void drain::EndPointWrite::setBufferSize(size_t _nbChunk) {
	m_bufferSizeMicroseconds = std11::chrono::microseconds(0);
	m_bufferSizeChunk = _nbChunk;
	if (    audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size() != 0
	     && m_output.getFrequency() != 0) {
		m_buffer.setCapacity(_nbChunk,
		                     audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size(),
		                     float(m_output.getFrequency()));
	}
}

void drain::EndPointWrite::setBufferSize(const std11::chrono::microseconds& _time) {
	m_bufferSizeMicroseconds = _time;
	m_bufferSizeChunk = 0;
	m_buffer.setCapacity(_time,
	                     audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size(),
	                     float(m_output.getFrequency()));
}

size_t drain::EndPointWrite::getBufferSize() {
	if (m_bufferSizeChunk != 0) {
		return m_bufferSizeChunk;
	}
	return (int64_t(m_output.getFrequency())*m_bufferSizeMicroseconds.count())/1000000LL;
}

std11::chrono::microseconds drain::EndPointWrite::getBufferSizeMicrosecond() {
	if (m_bufferSizeMicroseconds != std11::chrono::microseconds(0) ) {
		return m_bufferSizeMicroseconds;
	}
	return std11::chrono::microseconds(m_bufferSizeChunk*1000000LL/int64_t(m_output.getFrequency()));
}

size_t drain::EndPointWrite::getBufferFillSize() {
	return m_buffer.getSize()/(audio::getFormatBytes(m_output.getFormat())*m_output.getMap().size());
}

std11::chrono::microseconds drain::EndPointWrite::getBufferFillSizeMicrosecond() {
	return std11::chrono::microseconds(getBufferFillSize()*1000000LL/int64_t(m_output.getFrequency()));
}
