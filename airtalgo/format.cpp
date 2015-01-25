/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <airtalgo/format.h>

std::ostream& airtalgo::operator <<(std::ostream& _os, enum airtalgo::format _obj) {
	_os << getFormatString(_obj);
	return _os;
}

std::string airtalgo::getFormatString(enum airtalgo::format _value) {
	switch (_value) {
		case format_int16:
			return "format_int16";
			break;
		case format_int16_on_int32:
			return "format_int16_on_int32";
			break;
		case format_int32:
			return "format_int32";
			break;
		case format_float:
			return "format_float";
			break;
	};
}

enum airtalgo::format airtalgo::getFormatFromString(const std::string& _value) {
	if (_value == "format_int16") {
		return format_int16;
	}
	if (_value == "format_int16_on_int32") {
		return format_int16_on_int32;
	}
	if (_value == "format_int32") {
		return format_int32;
	}
	if (_value == "format_float") {
		return format_float;
	}
	return format_int16;
}