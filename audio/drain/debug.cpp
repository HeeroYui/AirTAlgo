/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/drain/debug.h>


int32_t audio::drain::getLogId() {
	static int32_t g_val = etk::log::registerInstance("audio-drain");
	return g_val;
}
