/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_CORE_FORMAT_H__
#define __AIRT_ALGO_CORE_FORMAT_H__

#include <string>

namespace airtalgo{
	enum format {
		format_int16, //!< Signed 16 bits
		format_int16_on_int32, //!< Signed 16 bits on 32bits data (16 bit fixpoint value)
		format_int32, //!< Signed 32 bits
		format_float, //!< Floating point (single precision)
	};
	std::string getFormatString(enum airtalgo::format);
	enum airtalgo::format getFormatFromString(const std::string& _value);
	std::ostream& operator <<(std::ostream& _os, enum airtalgo::format _obj);
};


#endif

