/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#ifndef __AIRT_ALGO_AUTO_LOG_IN_OUT_H__
#define __AIRT_ALGO_AUTO_LOG_IN_OUT_H__

#include <string>
#include "debug.h"

namespace airtalgo{
	class AutoLogInOut {
		private:
			std::string m_value;
		public:
			AutoLogInOut(const std::string& _value);
			~AutoLogInOut();
	};
}

#include "debugRemove.h"

#endif
