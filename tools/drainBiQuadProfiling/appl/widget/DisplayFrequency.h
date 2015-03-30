/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */


#ifndef __APPL_WIDGET_DISPLAY_FREQUENCY_H__
#define __APPL_WIDGET_DISPLAY_FREQUENCY_H__

#include <ewol/widget/Widget.h>
#include <ewol/compositing/Drawing.h>
#include <ewol/compositing/Text.h>
#include <ewol/widget/Manager.h>

namespace appl {
	namespace widget {
		class DisplayFrequency : public ewol::Widget {
			private:
				ewol::compositing::Drawing m_draw; //!< drawing instance
				ewol::compositing::Text m_text; //!< drawing instance
			protected:
				//! @brief constructor
				DisplayFrequency();
				void init();
			public:
				DECLARE_WIDGET_FACTORY(DisplayFrequency, "DisplayFrequency");
				//! @brief destructor
				virtual ~DisplayFrequency();
			private:
				std::vector<std::vector<std::pair<float,float> > > m_data; //!< data that might be displayed
			public:
				void clear() {
					m_data.clear();
				}
				void setValue(const std::vector<std::pair<float,float> >& _data);
			private:
				float m_gainMin; //!< display minimum gain value
				float m_gainMax; //!< display maximum gain value
			public:
				void setGainRange(float _min, float _max);
			private:
				float m_frequencyMin; //!< display minimum gain value
				float m_frequencyMax; //!< display maximum gain value
			public:
				void setFrequencyRange(float _min, float _max);
			public: // herited function
				virtual void onDraw();
				virtual void onRegenerateDisplay();
			private:
				vec2 m_borderSize;
		};
	}
}

#endif
