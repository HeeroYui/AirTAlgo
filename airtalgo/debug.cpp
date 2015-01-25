/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <airtalgo/debug.h>


int32_t airtalgo::getLogId() {
	static int32_t g_val = etk::log::registerInstance("airtalgo");
	return g_val;
}
