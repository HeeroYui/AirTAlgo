/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include "AutoLogInOut.hpp"
#include "debug.hpp"

audio::drain::AutoLogInOut::AutoLogInOut(const etk::String& _value) :
  m_value(_value) {
	DRAIN_VERBOSE("                 '" << m_value << "' [START]");
}

audio::drain::AutoLogInOut::~AutoLogInOut() {
	DRAIN_VERBOSE("                 '" << m_value << "' [STOP]");
}
