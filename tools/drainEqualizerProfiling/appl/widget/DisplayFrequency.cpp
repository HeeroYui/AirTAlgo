/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <appl/widget/DisplayFrequency.h>
#include <appl/debug.h>
#include <etk/tool.h>

#undef __class__
#define __class__ "DisplayFrequency"


appl::widget::DisplayFrequency::DisplayFrequency() :
  m_gainMin(0.0f),
  m_gainMax(1.0f),
  m_frequencyMin(0.0f),
  m_frequencyMax(24000.0f) {
	addObjectType("appl::widget::DisplayFrequency");
}

void appl::widget::DisplayFrequency::init() {
	ewol::Widget::init();
	markToRedraw();
}


appl::widget::DisplayFrequency::~DisplayFrequency() {
	
}


void appl::widget::DisplayFrequency::setValue(const std::vector<std::pair<float,float> >& _data) {
	m_data = _data;
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
}


void appl::widget::DisplayFrequency::onRegenerateDisplay() {
	//!< Check if we really need to redraw the display, if not needed, we redraw the previous data ...
	if (needRedraw() == false) {
		return;
	}
	// remove previous data
	m_draw.clear();
	// set background
	m_draw.setColor(etk::color::black);
	m_draw.setPos(vec2(0,0));
	m_draw.rectangleWidth(m_size);

	if (m_data.size() == 0) {
		return;
	}
	// set all the line:
	m_draw.setColor(etk::color::white);
	m_draw.setThickness(1);
	
	float ratioX = m_size.x() / (m_frequencyMax - m_frequencyMin);
	float ratioY = m_size.y() / (m_gainMax - m_gainMin);
	m_draw.setPos(vec2(ratioX*(m_data[0].first - m_gainMin),
	                   ratioY*(m_data[0].second - m_gainMin)));
	float baseX = 0;
	for (size_t iii=1; iii<m_data.size(); ++iii) {
		m_draw.lineTo(vec2(ratioX*(m_data[0].first - m_gainMin),
		                   ratioY*(m_data[0].second - m_gainMin)));
	}
}
