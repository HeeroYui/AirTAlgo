/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <ewol/widget/Windows.hpp>
#include <ewol/widget/Layer.hpp>
#include <ewol/widget/Composer.hpp>
#include <audio/drain/Equalizer.hpp>
#include <appl/widget/DisplayFrequency.hpp>

namespace appl {
	class Windows : public ewol::widget::Windows {
		protected:
			Windows();
			void init();
		public:
			DECLARE_FACTORY(Windows);
		protected:
			ememory::SharedPtr<ewol::widget::Composer> m_gui;
			ememory::SharedPtr<appl::widget::DisplayFrequency> m_displayer;
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
			ememory::SharedPtr<audio::drain::Equalizer> createEqualizer(enum audio::format _format = audio::format_float);
	};
}

