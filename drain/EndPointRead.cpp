/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <drain/EndPointRead.h>

#undef __class__
#define __class__ "EndPointRead"

drain::EndPointRead::EndPointRead() {
	
}


void drain::EndPointRead::init() {
	drain::EndPoint::init();
	m_type = "EndPointRead";
}

std11::shared_ptr<drain::EndPointRead> drain::EndPointRead::create() {
	std11::shared_ptr<drain::EndPointRead> tmp(new drain::EndPointRead());
	tmp->init();
	return tmp;
}

void drain::EndPointRead::configurationChange() {
	drain::EndPoint::configurationChange();
	m_needProcess = true;
}


bool drain::EndPointRead::process(std11::chrono::system_clock::time_point& _time,
                                  void* _input,
                                  size_t _inputNbChunk,
                                  void*& _output,
                                  size_t& _outputNbChunk){
	drain::AutoLogInOut tmpLog("EndPointRead");
	return false;
}

void drain::EndPointRead::setBufferSize(size_t _nbChunk) {
	DRAIN_TODO("...");
}

void drain::EndPointRead::setBufferSize(const std11::chrono::microseconds& _time) {
	DRAIN_TODO("...");
}

size_t drain::EndPointRead::getBufferSize() {
	DRAIN_TODO("...");
	return 0;
}

std11::chrono::microseconds drain::EndPointRead::getBufferSizeMicrosecond() {
	DRAIN_TODO("...");
	return std11::chrono::microseconds(0);
}

size_t drain::EndPointRead::getBufferFillSize() {
	DRAIN_TODO("...");
	return 0;
}

std11::chrono::microseconds drain::EndPointRead::getBufferFillSizeMicrosecond() {
	DRAIN_TODO("...");
	return std11::chrono::microseconds(0);
}

