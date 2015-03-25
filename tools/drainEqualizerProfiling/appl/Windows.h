/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __APPL_WINDOWS_H__
#define __APPL_WINDOWS_H__

#include <ewol/widget/Windows.h>
#include <ewol/widget/Layer.h>
#include <ewol/widget/Composer.h>

namespace appl {
	class Windows : public ewol::widget::Windows {
		protected:
			Windows();
			void init();
		public:
			DECLARE_FACTORY(Windows);
		protected:
			std::shared_ptr<ewol::widget::Composer> m_gui;
			void onCallbackFrequency(const std::string& _value);
	};
};


#endif