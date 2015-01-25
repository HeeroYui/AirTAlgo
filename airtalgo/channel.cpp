/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <airtalgo/channel.h>
#include "debug.h"


std::ostream& airtalgo::operator <<(std::ostream& _os, enum airtalgo::channel _obj) {
	_os << getChannelString(_obj);
	return _os;
}

std::string airtalgo::getChannelString(enum airtalgo::channel _value) {
	switch (_value) {
		case channel_frontLeft:
			return "front-left";
			break;
		case channel_frontCenter:
			return "front-center";
			break;
		case channel_frontRight:
			return "front-right";
			break;
		case channel_rearLeft:
			return "rear-left";
			break;
		case channel_rearCenter:
			return "rear-center";
			break;
		case channel_rearRight:
			return "rear-right";
			break;
		case channel_surroundLeft:
			return "surround-left";
			break;
		case channel_surroundRight:
			return "surround-right";
			break;
		case channel_subWoofer:
			return "sub-woofer";
			break;
		case channel_lfe:
			return "lfe";
			break;
	};
}

std::string airtalgo::getChannelString(const std::vector<enum airtalgo::channel>& _value) {
	std::string out;
	for (size_t iii=0; iii<_value.size(); ++iii) {
		if (iii != 0) {
			out += ";";
		}
		out += getChannelString(_value[iii]);
	}
	return out;
}


std::ostream& airtalgo::operator <<(std::ostream& _os, std::vector<enum airtalgo::channel> _obj) {
	_os << std::string("{");
	for (size_t iii=0; iii<_obj.size(); ++iii) {
		if (iii!=0) {
			_os << std::string(";");
		}
		_os << _obj[iii];
	}
	_os << std::string("}");
	return _os;
}

static std::vector<std::string> split(const std::string& _input, char _val) {
	std::vector<std::string> list;
	size_t lastStartPos = 0;
	for(size_t iii=0; iii<_input.size(); iii++) {
		if (_input[iii]==_val) {
			list.push_back(std::string(_input, lastStartPos, iii - lastStartPos));
			lastStartPos = iii+1;
		}
	}
	if (lastStartPos<_input.size()) {
		list.push_back(std::string(_input, lastStartPos));
	}
	return list;
}

std::vector<enum airtalgo::channel> airtalgo::getChannelFromString(const std::string& _value) {
	std::vector<enum airtalgo::channel> out;
	std::vector<std::string> listIO = split(_value, ';');
	for (size_t iii=0; iii<listIO.size(); ++iii) {
		if (listIO[iii] == "front-left") {
			out.push_back(channel_frontLeft);
		} else if (listIO[iii] == "front-right") {
			out.push_back(channel_frontRight);
		} else if (listIO[iii] == "front-center") {
			out.push_back(channel_frontCenter);
		} else if (listIO[iii] == "rear-left") {
			out.push_back(channel_rearLeft);
		} else if (listIO[iii] == "rear-right") {
			out.push_back(channel_rearRight);
		} else if (listIO[iii] == "rear-center") {
			out.push_back(channel_rearCenter);
		} else if (listIO[iii] == "surround-right") {
			out.push_back(channel_surroundLeft);
		} else if (listIO[iii] == "surround-left") {
			out.push_back(channel_surroundRight);
		} else if (listIO[iii] == "lfe") {
			out.push_back(channel_lfe);
		} else if (listIO[iii] == "sub-woofer") {
			out.push_back(channel_subWoofer);
		} else {
			//ROS_ERROR("Unknow: '%s' in [front-left;front-right;front-center;rear-left;rear-right;rear-center;surround-right;surround-left;lfe;subwoofer]", listIO[iii].c_str());
		}
	}
	return out;
}
