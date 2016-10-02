/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <string>
#include "debug.hpp"

namespace audio {
	namespace drain{
		class AutoLogInOut {
			private:
				std::string m_value;
			public:
				AutoLogInOut(const std::string& _value);
				~AutoLogInOut();
		};
	}
}

#include "debugRemove.hpp"

