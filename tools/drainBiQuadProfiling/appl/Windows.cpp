/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <ewol/ewol.hpp>
#include <appl/debug.hpp>
#include <appl/Windows.hpp>
#include <ewol/widget/Label.hpp>
#include <ewol/widget/Entry.hpp>
#include <ewol/widget/Button.hpp>
#include <ewol/widget/Slider.hpp>
#include <audio/drain/Equalizer.hpp>

define APPL_SAVE_FILE_MACRO(type,fileName,dataPointer,nbElement) \
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
	m_listSampleRate.pushBack(192000);
	m_listSampleRate.pushBack(176400);
	m_listSampleRate.pushBack(96000);
	m_listSampleRate.pushBack(88200);
	m_listSampleRate.pushBack(48000);
	m_listSampleRate.pushBack(44100);
	m_listSampleRate.pushBack(32000);
	m_listSampleRate.pushBack(22050);
	m_listSampleRate.pushBack(16000);
	m_listSampleRate.pushBack(11025);
	m_listSampleRate.pushBack(8000);
	m_listSampleRate.pushBack(4000);
	m_listType.pushBack(audio::algo::drain::biQuadType_none);
	m_listType.pushBack(audio::algo::drain::biQuadType_lowPass);
	m_listType.pushBack(audio::algo::drain::biQuadType_highPass);
	m_listType.pushBack(audio::algo::drain::biQuadType_bandPass);
	m_listType.pushBack(audio::algo::drain::biQuadType_notch);
	m_listType.pushBack(audio::algo::drain::biQuadType_peak);
	m_listType.pushBack(audio::algo::drain::biQuadType_lowShelf);
	m_listType.pushBack(audio::algo::drain::biQuadType_highShelf);
}

void appl::Windows::init() {
	ewol::widget::Windows::init();
	setTitle("Drain Equalizer Profiler");
	
	m_gui = ewol::widget::Composer::create(ewol::widget::Composer::file, "DATA:gui.xml");
	if (m_gui != nullptr) {
		setSubWidget(m_gui);
	}
	subBind(ewol::widget::Button, "sample-rate-low", signalPressed, sharedFromThis(), &appl::Windows::onCallbackSampleRateLow);
	subBind(ewol::widget::Button, "sample-rate-up", signalPressed, sharedFromThis(), &appl::Windows::onCallbackSampleRateUp);
	subBind(ewol::widget::Button, "type-low", signalPressed, sharedFromThis(), &appl::Windows::onCallbackTypeLow);
	subBind(ewol::widget::Button, "type-up", signalPressed, sharedFromThis(), &appl::Windows::onCallbackTypeUp);
	
	subBind(ewol::widget::Entry, "gain", signalModify, sharedFromThis(), &appl::Windows::onCallbackGain);
	subBind(ewol::widget::Slider, "gain-slider", signalChange, sharedFromThis(), &appl::Windows::onCallbackGainSlider);
	
	subBind(ewol::widget::Entry, "frequency", signalModify, sharedFromThis(), &appl::Windows::onCallbackFrequency);
	subBind(ewol::widget::Slider, "frequency-slider", signalChange, sharedFromThis(), &appl::Windows::onCallbackFrequencySlider);
	
	subBind(ewol::widget::Entry, "quality", signalModify, sharedFromThis(), &appl::Windows::onCallbackQuality);
	subBind(ewol::widget::Slider, "quality-slider", signalChange, sharedFromThis(), &appl::Windows::onCallbackQualitySlider);
	
	subBind(ewol::widget::Button, "display16", signalPressed, sharedFromThis(), &appl::Windows::onCallbackStart16);
	subBind(ewol::widget::Button, "displayFloat", signalPressed, sharedFromThis(), &appl::Windows::onCallbackStartFloat);
	m_displayer = ememory::dynamicPointerCast<appl::widget::DisplayFrequency>(getSubObjectNamed("displayer"));
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
			ewol::parameterSetOnObjectNamed("sample-rate", "value", etk::toString(m_sampleRate));
			ewol::parameterSetOnObjectNamed("frequency-slider", "max", etk::toString(m_sampleRate/2));
			onCallbackStart();
			return;
		}
	}
	m_sampleRate = m_listSampleRate[0];
	ewol::parameterSetOnObjectNamed("sample-rate", "value", etk::toString(m_sampleRate));
	ewol::parameterSetOnObjectNamed("frequency-slider", "max", etk::toString(m_sampleRate/2));
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
			ewol::parameterSetOnObjectNamed("sample-rate", "value", etk::toString(m_sampleRate));
			ewol::parameterSetOnObjectNamed("frequency-slider", "max", etk::toString(m_sampleRate/2));
			onCallbackStart();
			return;
		}
	}
	m_sampleRate = m_listSampleRate[0];
	ewol::parameterSetOnObjectNamed("sample-rate", "value", etk::toString(m_sampleRate));
	ewol::parameterSetOnObjectNamed("frequency-slider", "max", etk::toString(m_sampleRate/2));
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
			ewol::parameterSetOnObjectNamed("type", "value", etk::toString(m_type));
			onCallbackStart();
			return;
		}
	}
	m_type = m_listType[0];
	ewol::parameterSetOnObjectNamed("type", "value", etk::toString(m_type));
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
			ewol::parameterSetOnObjectNamed("type", "value", etk::toString(m_type));
			onCallbackStart();
			return;
		}
	}
	m_type = m_listType[0];
	ewol::parameterSetOnObjectNamed("type", "value", etk::toString(m_type));
	onCallbackStart();
}


void appl::Windows::onCallbackGain(const etk::String& _value) {
	m_gain = etk::string_to_float(_value);
	ewol::parameterSetOnObjectNamed("gain-slider", "value", etk::toString(_value));
	APPL_INFO("Gain " << m_gain);
	onCallbackStart();
}

void appl::Windows::onCallbackGainSlider(const float& _value) {
	m_gain = _value;
	ewol::parameterSetOnObjectNamed("gain", "value", etk::toString(_value));
	APPL_INFO("Gain " << m_gain);
	onCallbackStart();
}



void appl::Windows::onCallbackQuality(const etk::String& _value) {
	m_quality = etk::string_to_float(_value);
	ewol::parameterSetOnObjectNamed("quality-slider", "value", etk::toString(_value));
	APPL_INFO("quality " << m_quality);
	onCallbackStart();
}

void appl::Windows::onCallbackQualitySlider(const float& _value) {
	m_quality = _value;
	ewol::parameterSetOnObjectNamed("quality", "value", etk::toString(_value));
	APPL_INFO("quality " << m_quality);
	onCallbackStart();
}


void appl::Windows::onCallbackFrequency(const etk::String& _value) {
	m_cutFrequency = etk::string_to_float(_value);
	ewol::parameterSetOnObjectNamed("frequency-slider", "value", etk::toString(_value));
	APPL_INFO("cut frequency " << m_cutFrequency);
	onCallbackStart();
}

void appl::Windows::onCallbackFrequencySlider(const float& _value) {
	m_cutFrequency = _value;
	ewol::parameterSetOnObjectNamed("frequency", "value", etk::toString(_value));
	APPL_INFO("cut frequency " << m_cutFrequency);
	onCallbackStart();
}

#include <appl/debug.hpp>
#include <audio/algo/drain/BiQuad.hpp>
#include <audio/float_t.hpp>
#include <audio/int16_16_t.hpp>

void appl::Windows::onCallbackStart() {
	APPL_INFO("start ");
	int32_t iii = 10;
	etk::Vector<audio::channel> map;
	map.pushBack(audio::channel_frontCenter);
	// audio::drain::IOFormatInterface format(map, audio::format_int16, m_sampleRate);
	// audio::drain::IOFormatInterface format(map, audio::format_float, m_sampleRate);
	// create biquad
	audio::algo::drain::BiQuad<audio::float_t> bq;
	// configure parameter
	bq.setBiquad(m_type, m_cutFrequency, m_quality, m_gain, m_sampleRate);
	etk::Vector<etk::Pair<float,float> > theory = bq.calculateTheory(m_sampleRate);
	m_displayer->clear();
	m_displayer->setValue(theory);
}


void appl::Windows::onCallbackStart16() {
	APPL_INFO("start ");
	// create biquad
	audio::algo::drain::BiQuad<audio::int16_16_t> bq;
	// configure parameter
	bq.setBiquad(m_type, m_cutFrequency, m_quality, m_gain, m_sampleRate);
	etk::Vector<etk::Pair<float,float> > pratic;
	size_t len = 512;
	for (size_t iii=1; iii < len; iii++) {
		float freq = iii / (len - 1.0) * m_sampleRate / 2.0;
		// To reset filter
		bq.reset();
		double m_phase = 0;
		double baseCycle = 2.0*M_PI/double(m_sampleRate) * double(freq);
		float gain = 0;
		etk::Vector<audio::int16_16_t> data;
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
			value = etk::max(value, data[iii]);
		}
		gain = 20.0 * std::log10(value.getDouble()/32000.0);
		APPL_VERBOSE("LEVEL " << iii << " out = " << value << " % : " << gain);
		pratic.pushBack(etk::makePair<float, float>(float(freq),float(gain)));
	}
	m_displayer->setValue(pratic);
}

void appl::Windows::onCallbackStartFloat() {
	APPL_INFO("start ");
	// create biquad
	audio::algo::drain::BiQuad<audio::float_t> bq;
	// configure parameter
	bq.setBiquad(m_type, m_cutFrequency, m_quality, m_gain, m_sampleRate);
	etk::Vector<etk::Pair<float,float> > pratic;
	size_t len = 512;
	for (size_t iii=1; iii < len; iii++) {
		float freq = iii / (len - 1.0) * m_sampleRate / 2.0;
		// To reset filter
		bq.reset();
		double m_phase = 0;
		double baseCycle = 2.0*M_PI/double(m_sampleRate) * double(freq);
		float gain = 0;
		etk::Vector<audio::float_t> data;
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
			value = etk::max(value, data[iii]);
		}
		gain = 20.0 * std::log10(value.getDouble()/1.0);
		APPL_VERBOSE("LEVEL " << iii << " out = " << value << " % : " << gain);
		pratic.pushBack(etk::makePair<float, float>(float(freq),float(gain)));
	}
	m_displayer->setValue(pratic);
}

