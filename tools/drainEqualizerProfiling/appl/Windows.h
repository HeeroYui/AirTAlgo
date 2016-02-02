/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */
#pragma once

#include <ewol/widget/Windows.h>
#include <ewol/widget/Layer.h>
#include <ewol/widget/Composer.h>
#include <audio/drain/Equalizer.h>
#include <appl/widget/DisplayFrequency.h>

namespace appl {
	class Windows : public ewol::widget::Windows {
		protected:
			Windows();
			void init();
		public:
			DECLARE_FACTORY(Windows);
		protected:
			std::shared_ptr<ewol::widget::Composer> m_gui;
			std::shared_ptr<appl::widget::DisplayFrequency> m_displayer;
			void onCallbackSampleRateLow();
			void onCallbackSampleRateUp();
			void onCallbackGain0(const float& _value) {
				onCallbackGain(_value, 0);
			}
			void onCallbackGain1(const float& _value) {
				onCallbackGain(_value, 1);
			}
			void onCallbackGain2(const float& _value) {
				onCallbackGain(_value, 2);
			}
			void onCallbackGain3(const float& _value) {
				onCallbackGain(_value, 3);
			}
			void onCallbackGain4(const float& _value) {
				onCallbackGain(_value, 4);
			}
			void onCallbackGain5(const float& _value) {
				onCallbackGain(_value, 5);
			}
			void onCallbackGain6(const float& _value) {
				onCallbackGain(_value, 6);
			}
			void onCallbackGain7(const float& _value) {
				onCallbackGain(_value, 7);
			}
			void onCallbackGain8(const float& _value) {
				onCallbackGain(_value, 8);
			}
			void onCallbackGain9(const float& _value) {
				onCallbackGain(_value, 9);
			}
			void onCallbackGain(const float& _value, int32_t _id);
			void onCallbackStart();
			void onCallbackStart16();
			void onCallbackStartFloat();
		protected:
			int32_t m_sampleRate;
			std::vector<int32_t> m_listSampleRate;
			
			std::vector<float> m_listGain;
			std11::shared_ptr<audio::drain::Equalizer> createEqualizer(enum audio::format _format = audio::format_float);
	};
}

