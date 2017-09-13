/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <appl/widget/DisplayFrequency.hpp>
#include <appl/debug.hpp>
#include <etk/tool.hpp>

appl::widget::DisplayFrequency::DisplayFrequency() :
  m_gainMin(0.0f),
  m_gainMax(1.0f),
  m_frequencyMin(0.0f),
  m_frequencyMax(24000.0f) {
	addObjectType("appl::widget::DisplayFrequency");
	m_text.setFontSize(13);
}

void appl::widget::DisplayFrequency::init() {
	ewol::Widget::init();
	markToRedraw();
}


appl::widget::DisplayFrequency::~DisplayFrequency() {
	
}


void appl::widget::DisplayFrequency::setValue(const etk::Vector<etk::Pair<float,float> >& _data) {
	m_data.pushBack(_data);
	markToRedraw();
}

void appl::widget::DisplayFrequency::setGainRange(float _min, float _max) {
	m_gainMin = _min;
	m_gainMax = _max;
	markToRedraw();
}

void appl::widget::DisplayFrequency::setFrequencyRange(float _min, float _max) {
	m_frequencyMin = _min;
	m_frequencyMax = _max;
	markToRedraw();
}

void appl::widget::DisplayFrequency::onDraw() {
	m_draw.draw();
	m_text.draw();
}


void appl::widget::DisplayFrequency::onRegenerateDisplay() {
	//!< Check if we really need to redraw the display, if not needed, we redraw the previous data ...
	if (needRedraw() == false) {
		return;
	}
	// remove previous data
	m_draw.clear();
	m_text.clear();
	m_borderSize = m_size * 0.05;
	// set background
	m_draw.setColor(etk::color::black);
	m_draw.setPos(vec2(0,0));
	m_draw.rectangleWidth(m_size);
	m_draw.setColor(etk::color::gray);
	m_draw.setPos(m_borderSize);
	m_draw.setThickness(1);
	vec2 plop = m_size-m_borderSize;
	m_draw.lineTo(vec2(m_borderSize.x(), plop.y()));
	m_draw.lineTo(m_size-m_borderSize);
	m_draw.lineTo(vec2(plop.x(), m_borderSize.y()));
	m_draw.lineTo(m_borderSize);
	if (m_data.size() == 0) {
		return;
	}
	// calculate min and Max :
	m_gainMax = -999999999.9;
	m_gainMin = 9999999999.9;
	m_frequencyMin = 99999999.0;
	m_frequencyMax = -99999999.0;
	bool displayLog = true;
	for (size_t kkk=0; kkk<m_data.size(); kkk++) {
		for (size_t iii=0; iii<m_data[kkk].size(); ++iii) {
			if (etk::abs(m_data[kkk][iii].second) != std::numeric_limits<float>::infinity()) {
				m_gainMax = etk::max(m_gainMax, m_data[kkk][iii].second);
				m_gainMin = etk::min(m_gainMin, m_data[kkk][iii].second);
				//APPL_INFO("plop " << m_data[kkk][iii].second);
			}
			if (displayLog == false) {
				if (etk::abs(m_data[kkk][iii].first) != std::numeric_limits<float>::infinity()) {
					m_frequencyMax = etk::max(m_frequencyMax, m_data[kkk][iii].first);
					m_frequencyMin = etk::min(m_frequencyMin, m_data[kkk][iii].first);
				}
			} else {
				if (etk::abs(m_data[kkk][iii].first) != std::numeric_limits<float>::infinity()) {
					if (m_data[kkk][iii].first == 0) {
						continue;
					}
					m_frequencyMax = etk::max(m_frequencyMax, std::log(m_data[kkk][iii].first));
					m_frequencyMin = etk::min(m_frequencyMin, std::log(m_data[kkk][iii].first));
					//APPL_INFO("plop " << m_data[kkk][iii].first << " " << std::log(m_data[kkk][iii].first));
				}
			}
		}
	}
	// TODO : limit unit at a unit value.
	
	m_gainMin = int32_t(m_gainMin - 1);
	m_gainMax = int32_t(m_gainMax + 1);
	/*
	for (size_t iii=0; iii<m_data[0].size() && m_data[1].size(); ++iii) {
		APPL_INFO(" f=" << m_data[0][iii].first << " val=" << m_data[0][iii].second << " f=" << m_data[1][iii].first << " val=" << m_data[1][iii].second);
	}
	*/
	// set all the line:
	m_draw.setThickness(1);
	m_draw.setColor(etk::color::red);
	
	float ratioX = (m_size.x()-m_borderSize.x()*2.0) / (m_frequencyMax - m_frequencyMin);
	float ratioY = (m_size.y()-m_borderSize.y()*2.0) / (m_gainMax - m_gainMin);
	
	m_draw.setPos(m_borderSize + vec2(0, ratioY*(- m_gainMin)));
	m_draw.lineTo(m_borderSize + vec2(ratioX*(m_frequencyMax - m_frequencyMin), ratioY*(- m_gainMin)));
	for (size_t kkk=0; kkk<m_data.size(); ++kkk) {
		if (kkk == 0) {
			m_draw.setColor(etk::color::green);
		} else if (kkk == 0) {
			m_draw.setColor(etk::color::orange);
		} else {
			m_draw.setColor(etk::color::red);
		}
		
		if (displayLog == false) {
			m_draw.setPos(   m_borderSize
			               + vec2(ratioX*(m_data[kkk][0].first - m_frequencyMin),
			                      ratioY*(m_data[kkk][0].second - m_gainMin)));
			float baseX = 0;
			for (size_t iii=1; iii<m_data[kkk].size(); ++iii) {
				m_draw.lineTo(   m_borderSize
				               + vec2(ratioX*(m_data[kkk][iii].first - m_frequencyMin),
				                      ratioY*(m_data[kkk][iii].second - m_gainMin)));
			}
		} else {
			m_draw.setPos(   m_borderSize
			               + vec2(ratioX*(std::log(m_data[kkk][0].first) - m_frequencyMin),
			                      ratioY*(m_data[kkk][0].second - m_gainMin)));
			float baseX = 0;
			for (size_t iii=1; iii<m_data[kkk].size(); ++iii) {
				m_draw.lineTo(   m_borderSize
				               + vec2(ratioX*(std::log(m_data[kkk][iii].first) - m_frequencyMin),
				                      ratioY*(m_data[kkk][iii].second - m_gainMin)));
			}
		}
	}
	m_text.setDefaultColorFg(etk::color::green);
	etk::String textToDisplay = etk::toString(m_frequencyMin) + " Hz";
	vec3 size = m_text.calculateSize(textToDisplay);
	m_text.setPos(vec2(m_borderSize.x(), m_borderSize.y()-size.y()));
	m_text.print(textToDisplay);
	
	textToDisplay = etk::toString(m_frequencyMax) + " Hz";
	size = m_text.calculateSize(textToDisplay);
	m_text.setPos(vec2(m_size.x()-m_borderSize.x()-size.x(), m_borderSize.y()-size.y()));
	m_text.print(textToDisplay);
	
	textToDisplay = etk::toString(m_gainMin) + " dB";
	size = m_text.calculateSize(textToDisplay);
	m_text.setPos(vec2(m_borderSize.x(), m_borderSize.y()));
	m_text.print(textToDisplay);
	
	textToDisplay = etk::toString(m_gainMax) + " dB";
	size = m_text.calculateSize(textToDisplay);
	m_text.setPos(vec2(m_borderSize.x(), m_size.y() - m_borderSize.y()));
	m_text.print(textToDisplay);
}
