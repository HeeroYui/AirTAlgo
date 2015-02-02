/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <airtalgo/EndPointRead.h>

#undef __class__
#define __class__ "EndPointRead"

airtalgo::EndPointRead::EndPointRead() {
	
}


void airtalgo::EndPointRead::init() {
	airtalgo::EndPoint::init();
	m_type = "EndPointRead";
}

std::shared_ptr<airtalgo::EndPointRead> airtalgo::EndPointRead::create() {
	std::shared_ptr<airtalgo::EndPointRead> tmp(new airtalgo::EndPointRead());
	tmp->init();
	return tmp;
}

void airtalgo::EndPointRead::configurationChange() {
	airtalgo::EndPoint::configurationChange();
	m_needProcess = true;
}


bool airtalgo::EndPointRead::process(std::chrono::system_clock::time_point& _time,
                                            void* _input,
                                            size_t _inputNbChunk,
                                            void*& _output,
                                            size_t& _outputNbChunk){
	airtalgo::AutoLogInOut tmpLog("EndPointRead");
	return false;
}

