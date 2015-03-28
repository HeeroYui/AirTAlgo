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
#include <ewol/widget/Slider.h>

#undef __class__
#define __class__ "Windows"

appl::Windows::Windows() :
  m_sampleRate(48000),
  m_type(drain::filterType::filterType_lowPass),
  m_cutFrequency(8000.0),
  m_gain(0.0),
  m_quality(0.707) {
	addObjectType("appl::Windows");
	m_listSampleRate.push_back(192000);
	m_listSampleRate.push_back(176400);
	m_listSampleRate.push_back(96000);
	m_listSampleRate.push_back(88200);
	m_listSampleRate.push_back(48000);
	m_listSampleRate.push_back(44100);
	m_listSampleRate.push_back(32000);
	m_listSampleRate.push_back(22050);
	m_listSampleRate.push_back(16000);
	m_listSampleRate.push_back(11025);
	m_listSampleRate.push_back(8000);
	m_listSampleRate.push_back(4000);
	m_listType.push_back(drain::filterType_none);
	m_listType.push_back(drain::filterType_lowPass);
	m_listType.push_back(drain::filterType_highPass);
	m_listType.push_back(drain::filterType_bandPass);
	m_listType.push_back(drain::filterType_notch);
	m_listType.push_back(drain::filterType_peak);
	m_listType.push_back(drain::filterType_lowShelf);
	m_listType.push_back(drain::filterType_highShelf);
}

void appl::Windows::init() {
	ewol::widget::Windows::init();
	setTitle("Drain Equalizer Profiler");
	
	m_gui = ewol::widget::Composer::create(ewol::widget::Composer::file, "DATA:gui.xml");
	if (m_gui != nullptr) {
		setSubWidget(m_gui);
	}
	subBind(ewol::widget::Button, "sample-rate-low", signalPressed, shared_from_this(), &appl::Windows::onCallbackSampleRateLow);
	subBind(ewol::widget::Button, "sample-rate-up", signalPressed, shared_from_this(), &appl::Windows::onCallbackSampleRateUp);
	subBind(ewol::widget::Button, "type-low", signalPressed, shared_from_this(), &appl::Windows::onCallbackTypeLow);
	subBind(ewol::widget::Button, "type-up", signalPressed, shared_from_this(), &appl::Windows::onCallbackTypeUp);
	
	subBind(ewol::widget::Entry, "gain", signalModify, shared_from_this(), &appl::Windows::onCallbackGain);
	subBind(ewol::widget::Slider, "gain-slider", signalChange, shared_from_this(), &appl::Windows::onCallbackGainSlider);
	
	subBind(ewol::widget::Entry, "frequency", signalModify, shared_from_this(), &appl::Windows::onCallbackFrequency);
	subBind(ewol::widget::Slider, "frequency-slider", signalChange, shared_from_this(), &appl::Windows::onCallbackFrequencySlider);
	
	subBind(ewol::widget::Entry, "quality", signalModify, shared_from_this(), &appl::Windows::onCallbackQuality);
	subBind(ewol::widget::Slider, "quality-slider", signalChange, shared_from_this(), &appl::Windows::onCallbackQualitySlider);
	
	subBind(ewol::widget::Button, "display", signalPressed, shared_from_this(), &appl::Windows::onCallbackStart);
	m_displayer = std11::dynamic_pointer_cast<appl::widget::DisplayFrequency>(getSubObjectNamed("displayer"));
	onCallbackStart();
}


void appl::Windows::onCallbackSampleRateUp() {
	for (int32_t iii=0; iii<m_listSampleRate.size(); ++iii) {
		if (m_sampleRate == m_listSampleRate[iii]) {
			iii++;
			if (iii<m_listSampleRate.size()) {
				m_sampleRate = m_listSampleRate[iii];
			} else {
				m_sampleRate = m_listSampleRate[0];
			}
			ewol::parameterSetOnObjectNamed("sample-rate", "value", etk::to_string(m_sampleRate));
			ewol::parameterSetOnObjectNamed("frequency-slider", "max", etk::to_string(m_sampleRate/2));
			onCallbackStart();
			return;
		}
	}
	m_sampleRate = m_listSampleRate[0];
	ewol::parameterSetOnObjectNamed("sample-rate", "value", etk::to_string(m_sampleRate));
	ewol::parameterSetOnObjectNamed("frequency-slider", "max", etk::to_string(m_sampleRate/2));
	onCallbackStart();
}

void appl::Windows::onCallbackSampleRateLow() {
	for (int32_t iii=0; iii<m_listSampleRate.size(); ++iii) {
		if (m_sampleRate == m_listSampleRate[iii]) {
			iii--;
			if (iii>=0) {
				m_sampleRate = m_listSampleRate[iii];
			} else {
				m_sampleRate = m_listSampleRate[m_listSampleRate.size()-1];
			}
			ewol::parameterSetOnObjectNamed("sample-rate", "value", etk::to_string(m_sampleRate));
			ewol::parameterSetOnObjectNamed("frequency-slider", "max", etk::to_string(m_sampleRate/2));
			onCallbackStart();
			return;
		}
	}
	m_sampleRate = m_listSampleRate[0];
	ewol::parameterSetOnObjectNamed("sample-rate", "value", etk::to_string(m_sampleRate));
	ewol::parameterSetOnObjectNamed("frequency-slider", "max", etk::to_string(m_sampleRate/2));
	onCallbackStart();
}

void appl::Windows::onCallbackTypeUp() {
	for (int32_t iii=0; iii<m_listType.size(); ++iii) {
		if (m_type == m_listType[iii]) {
			iii++;
			if (iii<m_listType.size()) {
				m_type = m_listType[iii];
			} else {
				m_type = m_listType[0];
			}
			ewol::parameterSetOnObjectNamed("type", "value", etk::to_string(m_type));
			onCallbackStart();
			return;
		}
	}
	m_type = m_listType[0];
	ewol::parameterSetOnObjectNamed("type", "value", etk::to_string(m_type));
	onCallbackStart();
}

void appl::Windows::onCallbackTypeLow() {
	for (int32_t iii=0; iii<m_listType.size(); ++iii) {
		if (m_type == m_listType[iii]) {
			iii--;
			if (iii>=0) {
				m_type = m_listType[iii];
			} else {
				m_type = m_listType[m_listType.size()-1];
			}
			ewol::parameterSetOnObjectNamed("type", "value", etk::to_string(m_type));
			onCallbackStart();
			return;
		}
	}
	m_type = m_listType[0];
	ewol::parameterSetOnObjectNamed("type", "value", etk::to_string(m_type));
	onCallbackStart();
}


void appl::Windows::onCallbackGain(const std::string& _value) {
	m_gain = etk::string_to_float(_value);
	ewol::parameterSetOnObjectNamed("gain-slider", "value", etk::to_string(_value));
	APPL_INFO("Gain " << m_gain);
	onCallbackStart();
}

void appl::Windows::onCallbackGainSlider(const float& _value) {
	m_gain = _value;
	ewol::parameterSetOnObjectNamed("gain", "value", etk::to_string(_value));
	APPL_INFO("Gain " << m_gain);
	onCallbackStart();
}



void appl::Windows::onCallbackQuality(const std::string& _value) {
	m_quality = etk::string_to_float(_value);
	ewol::parameterSetOnObjectNamed("quality-slider", "value", etk::to_string(_value));
	APPL_INFO("quality " << m_quality);
	onCallbackStart();
}

void appl::Windows::onCallbackQualitySlider(const float& _value) {
	m_gain = _value;
	ewol::parameterSetOnObjectNamed("quality", "value", etk::to_string(_value));
	APPL_INFO("quality " << m_quality);
	onCallbackStart();
}


void appl::Windows::onCallbackFrequency(const std::string& _value) {
	m_cutFrequency = etk::string_to_float(_value);
	ewol::parameterSetOnObjectNamed("frequency-slider", "value", etk::to_string(_value));
	APPL_INFO("cut frequency " << m_cutFrequency);
	onCallbackStart();
}

void appl::Windows::onCallbackFrequencySlider(const float& _value) {
	m_cutFrequency = _value;
	ewol::parameterSetOnObjectNamed("frequency", "value", etk::to_string(_value));
	APPL_INFO("cut frequency " << m_cutFrequency);
	onCallbackStart();
}

#include <river/debug.h>

void appl::Windows::onCallbackStart() {
	APPL_INFO("start ");
	int32_t iii = 10;
	std::vector<audio::channel> map;
	map.push_back(audio::channel_frontCenter);
	//drain::IOFormatInterface format(map, audio::format_int16, m_sampleRate);
	drain::IOFormatInterface format(map, audio::format_float, m_sampleRate);
	// create equalizer
	std11::shared_ptr<drain::Equalizer> eq = drain::Equalizer::create();
	// configure input
	eq->setInputFormat(format);
	// configure output
	eq->setOutputFormat(format);
	// configure parameter
	eq->calcBiquad(m_type, m_cutFrequency, m_quality, m_gain);
	std::vector<std::pair<float,float> > theory = calculateTheory(m_sampleRate, eq->getCoef());
	m_displayer->clear();
	m_displayer->setValue(theory);
	std::vector<std::pair<float,float> > pratic;
	size_t len = 512;
	for (size_t iii=0; iii < len; iii++) {
		float freq = iii / (len - 1.0) * m_sampleRate / 2.0;
		// To reset filter
		eq->setParameter("reset", "");
		double m_phase = 0;
		double baseCycle = 2.0*M_PI/double(m_sampleRate) * double(freq);
		float gain = 0;
		if (format.getFormat() == audio::format_int16) {
			std::vector<int16_t> data;
			// create sinus
			data.resize(16000, 0);
			for (int32_t iii=0; iii<data.size(); iii++) {
				data[iii] = cos(m_phase) * 32000;
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
			//RIVER_SAVE_FILE_MACRO(int16_t,"aaa_test_INPUT.raw",&data[0],data.size());
			eq->process(time, &data[0], data.size(), outputVoid, outputNbChunk);
			output = static_cast<int16_t*>(outputVoid);
			//RIVER_SAVE_FILE_MACRO(int16_t,"aaa_test_OUTPUT.raw",output,outputNbChunk);
			int16_t value = 0;
			for (size_t iii=200; iii<outputNbChunk-200; ++iii) {
				value = std::max(value, output[iii]);
			}
			gain = 20.0 * std::log10(double(value)/32000.0);
			APPL_VERBOSE("LEVEL " << iii << " out = " << value << " % : " << gain);
		} else if (format.getFormat() == audio::format_float) {
			std::vector<float> data;
			// create sinus
			data.resize(16000, 0);
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
			//RIVER_SAVE_FILE_MACRO(int16_t,"aaa_test_INPUT.raw",&data[0],data.size());
			eq->process(time, &data[0], data.size(), outputVoid, outputNbChunk);
			output = static_cast<float*>(outputVoid);
			//RIVER_SAVE_FILE_MACRO(int16_t,"aaa_test_OUTPUT.raw",output,outputNbChunk);
			float value = 0;
			for (size_t iii=200; iii<outputNbChunk-200; ++iii) {
				value = std::max(value, output[iii]);
			}
			gain = 20.0 * std::log10(double(value)/1.0);
			APPL_VERBOSE("LEVEL " << iii << " out = " << value << " % : " << gain);
		}
		pratic.push_back(std::make_pair<float, float>(float(freq),float(gain)));
	}
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
		double freq = iii / (len - 1.0) * _sampleRate / 2.0;
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
		
		APPL_DEBUG("theory = " << freq << " power=" << y);
		out.push_back(std::make_pair<float,float>(freq, y));
	}
	return out;
}
