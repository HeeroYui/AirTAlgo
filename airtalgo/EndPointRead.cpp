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

airtalgo::EndPointRead::~EndPointRead() {
	AIRTALGO_INFO("Remove EndPointRead");
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
	airtalgo::autoLogInOut("EndPointRead");
	return false;
}

