/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <audio/drain/EndPointRead.hpp>
#include <audio/drain/debug.hpp>


audio::drain::EndPointRead::EndPointRead() {
	
}


void audio::drain::EndPointRead::init() {
	audio::drain::EndPoint::init();
	m_type = "EndPointRead";
}

ememory::SharedPtr<audio::drain::EndPointRead> audio::drain::EndPointRead::create() {
	ememory::SharedPtr<audio::drain::EndPointRead> tmp(ETK_NEW(audio::drain::EndPointRead));
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

void audio::drain::EndPointRead::setBufferSize(const echrono::microseconds& _time) {
	DRAIN_TODO("...");
}

size_t audio::drain::EndPointRead::getBufferSize() {
	DRAIN_TODO("...");
	return 0;
}

echrono::microseconds audio::drain::EndPointRead::getBufferSizeMicrosecond() {
	DRAIN_TODO("...");
	return echrono::microseconds(0);
}

size_t audio::drain::EndPointRead::getBufferFillSize() {
	DRAIN_TODO("...");
	return 0;
}

echrono::microseconds audio::drain::EndPointRead::getBufferFillSizeMicrosecond() {
	DRAIN_TODO("...");
	return echrono::microseconds(0);
}

