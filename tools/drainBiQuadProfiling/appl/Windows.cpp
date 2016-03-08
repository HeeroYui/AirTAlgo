/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <etk/types.h>
#include <ewol/ewol.h>
#include <appl/debug.h>
#include <appl/Windows.h>
#include <ewol/widget/Label.h>
#include <ewol/widget/Entry.h>
#include <ewol/widget/Button.h>
#include <ewol/widget/Slider.h>
#include <audio/drain/Equalizer.h>

#undef __class__
#define __class__ "Windows"

#define APPL_SAVE_FILE_MACRO(type,fileName,dataPointer,nbElement) \
	do { \
		static FILE *pointerOnFile = nullptr; \
		static bool errorOpen = false; \
		if (pointerOnFile == nullptr) { \
			APPL_WARNING("open file '" << fileName << "' type=" << #type); \
			pointerOnFile = fopen(fileName,"w"); \
			if (    errorOpen == false \
			     && pointerOnFile == nullptr) { \
				APPL_ERROR("ERROR OPEN file ... '" << fileName << "' type=" << #type); \
				errorOpen=true; \
			} \
		} \
		if (pointerOnFile != nullptr) { \
			fwrite((dataPointer), sizeof(type), (nbElement), pointerOnFile); \
			/* fflush(pointerOnFile);*/ \
		} \
	}while(0)

appl::Windows::Windows() :
  m_sampleRate(48000),
  m_type(audio::algo::drain::biQuadType_lowPass),
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
	m_listType.push_back(audio::algo::drain::biQuadType_none);
	m_listType.push_back(audio::algo::drain::biQuadType_lowPass);
	m_listType.push_back(audio::algo::drain::biQuadType_highPass);
	m_listType.push_back(audio::algo::drain::biQuadType_bandPass);
	m_listType.push_back(audio::algo::drain::biQuadType_notch);
	m_listType.push_back(audio::algo::drain::biQuadType_peak);
	m_listType.push_back(audio::algo::drain::biQuadType_lowShelf);
	m_listType.push_back(audio::algo::drain::biQuadType_highShelf);
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
	
	subBind(ewol::widget::Button, "display16", signalPressed, shared_from_this(), &appl::Windows::onCallbackStart16);
	subBind(ewol::widget::Button, "displayFloat", signalPressed, shared_from_this(), &appl::Windows::onCallbackStartFloat);
	m_displayer = std::dynamic_pointer_cast<appl::widget::DisplayFrequency>(getSubObjectNamed("displayer"));
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
	m_quality = _value;
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

#include <appl/debug.h>
#include <audio/algo/drain/BiQuad.h>
#include <audio/float_t.h>
#include <audio/int16_16_t.h>

void appl::Windows::onCallbackStart() {
	APPL_INFO("start ");
	int32_t iii = 10;
	std::vector<audio::channel> map;
	map.push_back(audio::channel_frontCenter);
	// audio::drain::IOFormatInterface format(map, audio::format_int16, m_sampleRate);
	// audio::drain::IOFormatInterface format(map, audio::format_float, m_sampleRate);
	// create biquad
	audio::algo::drain::BiQuad<audio::float_t> bq;
	// configure parameter
	bq.setBiquad(m_type, m_cutFrequency, m_quality, m_gain, m_sampleRate);
	std::vector<std::pair<float,float> > theory = bq.calculateTheory(m_sampleRate);
	m_displayer->clear();
	m_displayer->setValue(theory);
}


void appl::Windows::onCallbackStart16() {
	APPL_INFO("start ");
	// create biquad
	audio::algo::drain::BiQuad<audio::int16_16_t> bq;
	// configure parameter
	bq.setBiquad(m_type, m_cutFrequency, m_quality, m_gain, m_sampleRate);
	std::vector<std::pair<float,float> > pratic;
	size_t len = 512;
	for (size_t iii=1; iii < len; iii++) {
		float freq = iii / (len - 1.0) * m_sampleRate / 2.0;
		// To reset filter
		bq.reset();
		double m_phase = 0;
		double baseCycle = 2.0*M_PI/double(m_sampleRate) * double(freq);
		float gain = 0;
		std::vector<audio::int16_16_t> data;
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
		APPL_SAVE_FILE_MACRO(int16_t,"aaa_test_INPUT_16.raw",&data[0],data.size());
		bq.process(&data[0], &data[0], data.size(), 1, 1);
		APPL_SAVE_FILE_MACRO(int16_t,"aaa_test_OUTPUT_16.raw",&data[0],data.size());
		audio::int16_16_t value = 0;
		for (size_t iii=800; iii<data.size()-200; ++iii) {
			value = std::max(value, data[iii]);
		}
		gain = 20.0 * std::log10(value.getDouble()/32000.0);
		APPL_VERBOSE("LEVEL " << iii << " out = " << value << " % : " << gain);
		pratic.push_back(std::make_pair<float, float>(float(freq),float(gain)));
	}
	m_displayer->setValue(pratic);
}

void appl::Windows::onCallbackStartFloat() {
	APPL_INFO("start ");
	// create biquad
	audio::algo::drain::BiQuad<audio::float_t> bq;
	// configure parameter
	bq.setBiquad(m_type, m_cutFrequency, m_quality, m_gain, m_sampleRate);
	std::vector<std::pair<float,float> > pratic;
	size_t len = 512;
	for (size_t iii=1; iii < len; iii++) {
		float freq = iii / (len - 1.0) * m_sampleRate / 2.0;
		// To reset filter
		bq.reset();
		double m_phase = 0;
		double baseCycle = 2.0*M_PI/double(m_sampleRate) * double(freq);
		float gain = 0;
		std::vector<audio::float_t> data;
		// create sinus
		data.resize(16000, 0);
		for (int32_t iii=0; iii<data.size(); iii++) {
			data[iii] = cos(m_phase);
			m_phase += baseCycle;
			if (m_phase >= 2*M_PI) {
				m_phase -= 2*M_PI;
			}
		}
		APPL_SAVE_FILE_MACRO(float,"aaa_test_INPUT_F.raw",&data[0],data.size());
		bq.process(&data[0], &data[0], data.size(), 1, 1);
		APPL_SAVE_FILE_MACRO(float,"aaa_test_OUTPUT_F.raw",&data[0],data.size());
		audio::float_t value = 0.0f;
		for (size_t iii=800; iii<data.size()-200; ++iii) {
			value = std::max(value, data[iii]);
		}
		gain = 20.0 * std::log10(value.getDouble()/1.0);
		APPL_VERBOSE("LEVEL " << iii << " out = " << value << " % : " << gain);
		pratic.push_back(std::make_pair<float, float>(float(freq),float(gain)));
	}
	m_displayer->setValue(pratic);
}

