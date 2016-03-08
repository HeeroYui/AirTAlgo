/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/drain/EndPointRead.h>
#include <audio/drain/debug.h>

#undef __class__
#define __class__ "EndPointRead"

audio::drain::EndPointRead::EndPointRead() {
	
}


void audio::drain::EndPointRead::init() {
	audio::drain::EndPoint::init();
	m_type = "EndPointRead";
}

std::shared_ptr<audio::drain::EndPointRead> audio::drain::EndPointRead::create() {
	std::shared_ptr<audio::drain::EndPointRead> tmp(new audio::drain::EndPointRead());
	tmp->init();
	return tmp;
}

void audio::drain::EndPointRead::configurationChange() {
	audio::drain::EndPoint::configurationChange();
	m_needProcess = true;
}


bool audio::drain::EndPointRead::process(audio::Time& _time,
                                         void* _input,
                                         size_t _inputNbChunk,
                                         void*& _output,
                                         size_t& _outputNbChunk){
	audio::drain::AutoLogInOut tmpLog("EndPointRead");
	return false;
}

void audio::drain::EndPointRead::setBufferSize(size_t _nbChunk) {
	DRAIN_TODO("...");
}

void audio::drain::EndPointRead::setBufferSize(const std::chrono::microseconds& _time) {
	DRAIN_TODO("...");
}

size_t audio::drain::EndPointRead::getBufferSize() {
	DRAIN_TODO("...");
	return 0;
}

std::chrono::microseconds audio::drain::EndPointRead::getBufferSizeMicrosecond() {
	DRAIN_TODO("...");
	return std::chrono::microseconds(0);
}

size_t audio::drain::EndPointRead::getBufferFillSize() {
	DRAIN_TODO("...");
	return 0;
}

std::chrono::microseconds audio::drain::EndPointRead::getBufferFillSizeMicrosecond() {
	DRAIN_TODO("...");
	return std::chrono::microseconds(0);
}

