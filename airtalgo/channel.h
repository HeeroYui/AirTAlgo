/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_CORE_CHANNEL_H__
#define __AIRT_ALGO_CORE_CHANNEL_H__

#include <string>
#include <vector>

namespace airtalgo{
	enum channel {
		channel_frontLeft, //!< channel Front Left
		channel_frontCenter, //!< channel Front Center
		channel_frontRight, //!< channel Front Right
		channel_rearLeft, //!< channel rear Left
		channel_rearCenter, //!< channel rear Center
		channel_rearRight, //!< channel rear Right
		channel_surroundLeft, //!< channel surround Left
		channel_surroundRight, //!< channel surround Right
		channel_subWoofer, //!< channel Sub-woofer
		channel_lfe, //!< channel Low frequency
	};
	std::string getChannelString(enum airtalgo::channel);
	std::string getChannelString(const std::vector<enum airtalgo::channel>&);
	std::vector<enum airtalgo::channel> getChannelFromString(const std::string& _value);
	std::ostream& operator <<(std::ostream& _os, enum airtalgo::channel _obj);
	std::ostream& operator <<(std::ostream& _os, const std::vector<enum airtalgo::channel>& _obj);
	std::ostream& operator <<(std::ostream& _os, const std::vector<std::vector<enum airtalgo::channel>>& _obj);
};


#endif
