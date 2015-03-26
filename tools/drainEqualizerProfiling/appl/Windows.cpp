/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <ewol/ewol.h>
#include <appl/debug.h>
#include <appl/Windows.h>
#include <ewol/widget/Label.h>
#include <ewol/widget/Entry.h>
#include <ewol/widget/Button.h>

#undef __class__
#define __class__ "Windows"

appl::Windows::Windows() :
  m_sampleRate(48000),
  m_type(drain::filterType::filterType_LPF),
  m_cutFrequency(2000),
  m_gain(2),
  m_bandWidth(200) {
	addObjectType("appl::Windows");
}

void appl::Windows::init() {
	ewol::widget::Windows::init();
	setTitle("Drain Equalizer Profiler");
	
	m_gui = ewol::widget::Composer::create(ewol::widget::Composer::file, "DATA:gui.xml");
	if (m_gui != nullptr) {
		setSubWidget(m_gui);
	}
	subBind(ewol::widget::Entry, "sample-rate", signalModify, shared_from_this(), &appl::Windows::onCallbackSampleRate);
	subBind(ewol::widget::Entry, "type", signalModify, shared_from_this(), &appl::Windows::onCallbackType);
	subBind(ewol::widget::Entry, "gain", signalModify, shared_from_this(), &appl::Windows::onCallbackGain);
	subBind(ewol::widget::Entry, "frequency", signalModify, shared_from_this(), &appl::Windows::onCallbackFrequency);
	subBind(ewol::widget::Entry, "band-width", signalModify, shared_from_this(), &appl::Windows::onCallbackBandWidth);
	subBind(ewol::widget::Button, "display", signalPressed, shared_from_this(), &appl::Windows::onCallbackStart);
}

void appl::Windows::onCallbackSampleRate(const std::string& _value) {
	APPL_INFO("SampleRate " << _value);
	std::vector<int32_t> list;
	list.push_back(96000);
	list.push_back(48000);
	list.push_back(44100);
	list.push_back(32000);
	list.push_back(22050);
	list.push_back(16000);
	list.push_back(8000);
	if (etk::isIn(etk::string_to_int32_t(_value), list) == true) {
		ewol::parameterSetOnObjectNamed("sample-rate-valid", "color", "green");
	} else {
		ewol::parameterSetOnObjectNamed("sample-rate-valid", "color", "red");
	}
}

void appl::Windows::onCallbackType(const std::string& _value) {
	APPL_INFO("Type " << _value);
	std::vector<std::string> list;
	list.push_back("HPF");
	list.push_back("LPF");
	if (etk::isIn(etk::string_to_int32_t(_value), list) == true) {
		ewol::parameterSetOnObjectNamed("type-valid", "color", "green");
	} else {
		ewol::parameterSetOnObjectNamed("type-valid", "color", "red");
	}
}

void appl::Windows::onCallbackGain(const std::string& _value) {
	APPL_INFO("Gain " << _value);
}

void appl::Windows::onCallbackFrequency(const std::string& _value) {
	APPL_INFO("Frequency " << _value);
}

void appl::Windows::onCallbackBandWidth(const std::string& _value) {
	APPL_INFO("BandWidth " << _value);
}

void appl::Windows::onCallbackStart() {
	APPL_INFO("start ");
	m_data.clear();
	int32_t iii = 200;
	while (iii < m_sampleRate/2) {
		APPl_INFO("TEST : " << iii);
		// create equalizer
		std11::shared_ptr<drain::Equalizer> eq = drain::Equalizer::create();
		// configure parameter
		eq->setParameter("type", "LPF");
		eq->setParameter("gain", etk::to_string(m_gain));
		eq->setParameter("frequency", etk::to_string(m_cutFrequency));
		eq->setParameter("band-width", etk::to_string(m_bandWidth));
		// configure input
		
		// configure outpur
		
		// create sinus
		
		// process
		
		iii += 200;
	}
	
}


