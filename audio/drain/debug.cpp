/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <audio/drain/debug.hpp>


int32_t audio::drain::getLogId() {
	static int32_t g_val = elog::registerInstance("audio-drain");
	return g_val;
}
