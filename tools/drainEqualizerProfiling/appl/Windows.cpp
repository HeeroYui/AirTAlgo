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
  m_type(drain::filterType::filterType_lowPass),
  m_cutFrequency(8000.0),
  m_gain(6.0),
  m_bandWidth(0.1) {
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
	m_displayer = std11::dynamic_pointer_cast<appl::widget::DisplayFrequency>(getSubObjectNamed("displayer"));
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
	if (etk::isIn(_value, list) == true) {
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

#include <river/debug.h>

void appl::Windows::onCallbackStart() {
	APPL_INFO("start ");
	int32_t iii = 10;
	std::vector<audio::channel> map;
	map.push_back(audio::channel_frontCenter);
	drain::IOFormatInterface format(map, audio::format_int16, m_sampleRate);
	// create equalizer
	std11::shared_ptr<drain::Equalizer> eq = drain::Equalizer::create();
	// configure input
	eq->setInputFormat(format);
	// configure output
	eq->setOutputFormat(format);
	// configure parameter
	/*
	eq->setParameter("type", "APF");
	eq->setParameter("gain", etk::to_string(m_gain));
	eq->setParameter("frequency", etk::to_string(m_cutFrequency));
	eq->setParameter("band-width", etk::to_string(m_bandWidth));
	*/
	//eq->calcBiquad(drain::filterType_none, m_cutFrequency, 0.707, m_gain);
	eq->calcBiquad(drain::filterType_lowPass, m_cutFrequency, 0.707, m_gain);
	//eq->calcBiquad(drain::filterType_highPass, m_cutFrequency, 0.707, m_gain);
	std::vector<std::pair<float,float> > theory = calculateTheory(48000, eq->getCoef());
	m_displayer->clear();
	m_displayer->setValue(theory);
	std::vector<std::pair<float,float> > pratic;
	size_t len = 512;
	for (size_t iii=0; iii < len; iii++) {
		double w;
		//if (buildLinear == true) {
			// 0 to pi, linear scale
			w = iii / (len - 1.0) * M_PI;
		//} else {
			// 0.001 to 1, times pi, log scale
		//	w = std::exp(std::log(1.0 / 0.001) * iii / (len - 1.0)) * 0.001 * M_PI;
		//}
		double freq = iii / (len - 1.0) * 48000 / 2.0;
		
	//while (iii < m_sampleRate/2) {
		std::vector<int16_t> data;
		//APPL_INFO("TEST : " << iii);
		// To reset filter
		eq->setParameter("reset", "");
		// create sinus
		data.resize(4096, 0);
		double m_phase = 0;
		double baseCycle = 2.0*M_PI/double(m_sampleRate) * double(freq);
		for (int32_t iii=0; iii<data.size(); iii++) {
			data[iii] = cos(m_phase) * 30000;
			m_phase += baseCycle;
			if (m_phase >= 2*M_PI) {
				m_phase -= 2*M_PI;
			}
		}
		// process
		int16_t* output = nullptr;
		void* outputVoid = nullptr;
		size_t outputNbChunk = 0;
		std11::chrono::system_clock::time_point time;
		RIVER_SAVE_FILE_MACRO(int16_t,"aaa_test_INPUT.raw",&data[0],data.size());
		eq->process(time, &data[0], data.size(), outputVoid, outputNbChunk);
		output = static_cast<int16_t*>(outputVoid);
		RIVER_SAVE_FILE_MACRO(int16_t,"aaa_test_OUTPUT.raw",output,outputNbChunk);
		int16_t value = 0;
		for (size_t iii=20; iii<outputNbChunk-20; ++iii) {
			value = std::max(value, output[iii]);
		}
		float gain = 20.0 * std::log10(double(value)/30000.0);
		APPL_INFO("LEVEL " << iii << " out = " << value << " % : " << gain);
		pratic.push_back(std::make_pair<float, float>(iii,float(value)/30000.0f));
		iii += 10;
	}
	/*
	while (iii < m_sampleRate/2) {
		std::vector<float> data;
		//APPL_INFO("TEST : " << iii);
		// To reset filter
		eq->setParameter("reset", "");
		// create sinus
		data.resize(4096, 0);
		double m_phase = 0;
		double baseCycle = 2.0*M_PI/double(m_sampleRate) * double(iii);
		for (int32_t iii=0; iii<data.size(); iii++) {
			data[iii] = cos(m_phase);
			m_phase += baseCycle;
			if (m_phase >= 2*M_PI) {
				m_phase -= 2*M_PI;
			}
		}
		// process
		float* output = nullptr;
		void* outputVoid = nullptr;
		size_t outputNbChunk = 0;
		std11::chrono::system_clock::time_point time;
		RIVER_SAVE_FILE_MACRO(float,"aaa_test_INPUT_F.raw",&data[0],data.size());
		eq->process(time, &data[0], data.size(), outputVoid, outputNbChunk);
		output = static_cast<float*>(outputVoid);
		RIVER_SAVE_FILE_MACRO(float,"aaa_test_OUTPUT_F.raw",output,outputNbChunk);
		double value = 0;
		for (size_t iii=0; iii<outputNbChunk; ++iii) {
			value += std::abs(output[iii]);
		}
		value /= (outputNbChunk);
		float gain = 20.0 * std::log10(float(value));
		APPL_INFO("LEVEL " << iii << " out = " << value << " % : " << gain);
		pratic.push_back(std::make_pair<float, float>(iii,float(value)));
		iii += 10;
	}
	*/
	m_displayer->setValue(pratic);
}



std::vector<std::pair<float,float> > appl::Windows::calculateTheory(double _sampleRate, std::vector<float> _coef) {
	std::vector<std::pair<float,float> > out;
	double norm;
	float m_a[3];
	float m_b[2];
	
	m_a[0] = _coef[0];
	m_a[1] = _coef[1];
	m_a[2] = _coef[2];
	m_b[0] = _coef[3];
	m_b[1] = _coef[4];
	
	bool buildLinear = true;
	
	size_t len = 512;
	for (size_t iii=0; iii < len; iii++) {
		double w;
		if (buildLinear == true) {
			// 0 to pi, linear scale
			w = iii / (len - 1.0) * M_PI;
		} else {
			// 0.001 to 1, times pi, log scale
			w = std::exp(std::log(1.0 / 0.001) * iii / (len - 1.0)) * 0.001 * M_PI;
		}
		double phi = std::pow(std::sin(w/2.0), 2.0);
		double y =   std::log(   std::pow(m_a[0]+m_a[1]+m_a[2], 2.0)
		                       - 4.0*(m_a[0]*m_a[1] + 4.0*m_a[0]*m_a[2] + m_a[1]*m_a[2])*phi
		                       + 16.0*m_a[0]*m_a[2]*phi*phi)
		           - std::log(   std::pow(1.0+m_b[0]+m_b[1], 2.0)
		                       - 4.0*(m_b[0] + 4.0*m_b[1] + m_b[0]*m_b[1])*phi
		                       + 16.0*m_b[1]*phi*phi);
		y = y * 10.0 / M_LN10;
		
		if (y <= -200) {
			y = -200.0;
		}
		
		APPL_INFO("theory = " << iii / (len - 1.0) * _sampleRate / 2.0 << " power=" << y);
		out.push_back(std::make_pair<float,float>(iii / (len - 1.0) * _sampleRate / 2.0, y + 0.5));
	}
	return out;
}
