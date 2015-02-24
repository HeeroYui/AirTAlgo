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

