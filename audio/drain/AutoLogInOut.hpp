/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/String.hpp>
#include "debug.hpp"

namespace audio {
	namespace drain{
		class AutoLogInOut {
			private:
				etk::String m_value;
			public:
				AutoLogInOut(const etk::String& _value);
				~AutoLogInOut();
		};
	}
}

#include "debugRemove.hpp"

