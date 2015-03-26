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
#include <drain/Equalizer.h>

namespace appl {
	class Windows : public ewol::widget::Windows {
		protected:
			Windows();
			void init();
		public:
			DECLARE_FACTORY(Windows);
		protected:
			std::shared_ptr<ewol::widget::Composer> m_gui;
			void onCallbackSampleRate(const std::string& _value);
			void onCallbackType(const std::string& _value);
			void onCallbackGain(const std::string& _value);
			void onCallbackFrequency(const std::string& _value);
			void onCallbackBandWidth(const std::string& _value);
			void onCallbackStart();
		protected:
			int32_t m_sampleRate;
			enum drain::filterType m_type;
			float m_cutFrequency;
			float m_gain;
			float m_bandWidth;
			std::vector<std::pair<float,float> > m_data;
	};
};


#endif