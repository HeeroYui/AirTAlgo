/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "AutoLogInOut.h"
#include "debug.h"

audio::drain::AutoLogInOut::AutoLogInOut(const std::string& _value) :
  m_value(_value) {
	DRAIN_VERBOSE("                 '" << m_value << "' [START]");
}

audio::drain::AutoLogInOut::~AutoLogInOut() {
	DRAIN_VERBOSE("                 '" << m_value << "' [STOP]");
}