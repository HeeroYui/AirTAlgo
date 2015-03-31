/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <drain/debug.h>
#include <drain/echoCanceller/Lms.h>

drain::Lms::Lms(void) :
  m_filtre(),
  m_feedBack(),
  m_micro(0.1f) {
	setFilterSize(256);
}

drain::Lms::~Lms(void) {
	
}

void drain::Lms::reset(void) {
	// simply reset filters.
	setFilterSize(m_filtre.size());
}

bool drain::Lms::process(int16_t* _output, int16_t* _feedback, int16_t* _microphone, int32_t _nbSample) {
	float output[_nbSample];
	float feedback[_nbSample];
	float microphone[_nbSample];
	for (size_t iii=0; iii<_nbSample; ++iii) {
		microphone[iii] = float(_microphone[iii])/32767.0f;
		feedback[iii] = float(_feedback[iii])/32767.0f;
	}
	bool ret = process(output, feedback, microphone, _nbSample);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		_output[iii] = int16_t(float(output[iii])*32767.0f);
	}
	return ret;
}

bool drain::Lms::process(float* _output, float* _feedback, float* _microphone, int32_t _nbSample) {
	// add sample in the feedback history:
	m_feedBack.resize(m_filtre.size(), 0.0f);
	memcpy(&m_feedBack[m_filtre.size()], _feedback, _nbSample*sizeof(float));
	for (int32_t iii=0; iii < _nbSample; iii++) {
		_output[iii] = processValue(&m_feedBack[m_filtre.size()+iii], _microphone[iii]);
	}
	// remove old value:
	m_feedBack.erase(m_feedBack.begin(), m_feedBack.begin() + (m_feedBack.size()-m_filtre.size()) );
	return true;
}


static float convolution(float* _dataMinus, float* _dataPlus, size_t _count) {
	float out = 0.0f;
	for (size_t iii = 0; iii < _count; ++iii) {
		out += *_dataMinus-- * *_dataPlus++;
	}
	return out;
}

static void updateFilter(float* _filter, float* _data, float _value, int32_t _count) {
	for (size_t iii = 0; iii < _count; ++iii) {
		*(_filter++) += *_data-- * _value;
	}
}

float drain::Lms::processValue(float* _feedback, float _microphone) {
	// Error calculation.
	float convolutionValue = convolution(_feedback, &m_filtre[0], m_filtre.size());
	float error = _microphone - convolutionValue;
	float out = std::avg(-1.0f, error, 1.0f);
	updateFilter(&m_filtre[0], _feedback, 2.0f*m_micro, m_filtre.size());
	return out;
}

void drain::Lms::setFilterSize(size_t _sampleRate, std11::chrono::microseconds _time) {
	setFilterSize((_sampleRate*_time.count())/1000000LL);
}

void drain::Lms::setFilterSize(size_t _nbSample) {
	m_filtre.clear();
	m_feedBack.clear();
	m_filtre.resize(_nbSample, 0.0f);
	m_feedBack.resize(_nbSample, 0.0f);
}

