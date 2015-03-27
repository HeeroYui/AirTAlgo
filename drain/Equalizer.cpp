/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <drain/debug.h>
#include <drain/Equalizer.h>
#include <drain/Algo.h>

// see http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
// see http://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/

drain::Equalizer::Equalizer() {
	
}

void drain::Equalizer::init() {
	drain::Algo::init();
	drain::Algo::m_type = "Equalizer";
	m_supportedFormat.push_back(audio::format_int16);
	m_supportedFormat.push_back(audio::format_float);
	m_type = drain::filterType_none;
	m_gain = 6;
	m_frequencyCut = 1000;
	m_qualityFactor = 0.707;
	configureBiQuad();
	// reset coefficients
	m_a[0] = 1.0;
	m_a[1] = 0.0;
	m_a[2] = 0.0;
	m_b[0] = 0.0;
	m_b[1] = 0.0;
}

std11::shared_ptr<drain::Equalizer> drain::Equalizer::create() {
	std11::shared_ptr<drain::Equalizer> tmp(new drain::Equalizer());
	tmp->init();
	return tmp;
}

drain::Equalizer::~Equalizer() {
	
}

void drain::Equalizer::configurationChange() {
	drain::Algo::configurationChange();
	// Resize the configuration ouput of algirithm
	m_history.clear();
	m_history.resize(getOutputFormat().getMap().size());
}

bool drain::Equalizer::process(std11::chrono::system_clock::time_point& _time,
                               void* _input,
                               size_t _inputNbChunk,
                               void*& _output,
                               size_t& _outputNbChunk) {
	_outputNbChunk = _inputNbChunk;
	_output = _input;
	if (_input == nullptr) {
		return false;
	}
	if (getOutputFormat().getFormat() == audio::format_float) {
		for (size_t jjj=0; jjj<getOutputFormat().getMap().size(); ++jjj) {
			// get pointer on data:
			float* data = static_cast<float*>(_input);
			// move to sample offset:
			data += jjj;
			for (size_t iii=0; iii<_inputNbChunk; ++iii) {
				// process in float the biquad.
				*data = processFloat(*data, m_history[jjj]);
				// move to the sample on the same channel.
				data += getOutputFormat().getMap().size();
			}
		}
	} else if (getOutputFormat().getFormat() == audio::format_int16) {
		for (size_t jjj=0; jjj<getOutputFormat().getMap().size(); ++jjj) {
			// get pointer on data:
			int16_t* data = static_cast<int16_t*>(_input);
			// move to sample offset:
			data += jjj;
			for (size_t iii=0; iii<_inputNbChunk; ++iii) {
				// process in float the biquad.
				float out = processFloat(*data, m_history[jjj]);
				// Limit output.
				out = std::avg(-32768.0f, out, 32767.0f);
				*data = static_cast<int16_t>(out);
				// move to the sample on the same channel.
				data += getOutputFormat().getMap().size();
			}
		}
	}
	return true;
}

bool drain::Equalizer::setParameter(const std::string& _parameter, const std::string& _value) {
	//DRAIN_WARNING("set : " << _parameter << " " << _value);
	if (_parameter == "type") {
		if (_value == "none") {
			m_type = drain::filterType_none;
		} else if (_value == "LPF") {
			m_type = drain::filterType_lowPass;
		} else if (_value == "HPF") {
			m_type = drain::filterType_highPass;
		} else if (_value == "BPF") {
			m_type = drain::filterType_bandPass;
		} else if (_value == "NOTCH") {
			m_type = drain::filterType_notch;
		} else if (_value == "PeakingEQ") {
			m_type = drain::filterType_peak;
		} else if (_value == "LSH") {
			m_type = drain::filterType_lowShelf;
		} else if (_value == "HSH") {
			m_type = drain::filterType_highShelf;
		} else {
			DRAIN_ERROR("Can not set equalizer type : " << _value);
			return false;
		}
		configureBiQuad();
		return true;
	} else if (_parameter == "gain") {
		m_gain = etk::string_to_double(_value);
		configureBiQuad();
		return true;
	} else if (_parameter == "frequency") {
		m_frequencyCut = etk::string_to_double(_value);
		configureBiQuad();
		return true;
	} else if (_parameter == "quality") {
		m_qualityFactor = etk::string_to_double(_value);
		configureBiQuad();
		return true;
	} else if (_parameter == "reset") {
		m_history.clear();
		m_history.resize(getOutputFormat().getMap().size());
		return true;
	}
	return false;
}

std::string drain::Equalizer::getParameter(const std::string& _parameter) const {
	return "error";
}

std::string drain::Equalizer::getParameterProperty(const std::string& _parameter) const {
	return "error";
}

float drain::Equalizer::processFloat(float _sample, drain::BGHistory& _history) {
	float result;
	// compute
	result =   m_a[0] * _sample
	         + m_a[1] * _history.m_x[0]
	         + m_a[2] * _history.m_x[1]
	         - m_b[0] * _history.m_y[0]
	         - m_b[1] * _history.m_y[1];
	//update history of X
	_history.m_x[1] = _history.m_x[0];
	_history.m_x[0] = _sample;
	//update history of Y
	_history.m_y[1] = _history.m_y[0];
	_history.m_y[0] = result;
	return result;
}

bool drain::Equalizer::configureBiQuad() {
	calcBiquad(m_type, m_frequencyCut, m_qualityFactor, m_gain);
	return true;
}

void drain::Equalizer::calcBiquad(enum drain::filterType _type, double _frequencyCut, double _qualityFactor, double _gain) {
	m_type = _type;
	m_frequencyCut = _frequencyCut;
	m_qualityFactor = _qualityFactor;
	m_gain = _gain;
	
	if (getOutputFormat().getFrequency() < 1) {
		m_a[0] = 1.0;
		m_a[1] = 0.0;
		m_a[2] = 0.0;
		m_b[0] = 0.0;
		m_b[1] = 0.0;
		return;
	}
	if (m_frequencyCut > getOutputFormat().getFrequency()/2) {
		m_frequencyCut = getOutputFormat().getFrequency()/2;
	} else if (m_frequencyCut < 0) {
		m_frequencyCut = 0;
	}
	if (m_qualityFactor < 0.01) {
		m_qualityFactor = 0.01;
	}
	switch (m_type) {
		case filterType_lowPass:
		case filterType_highPass:
		case filterType_bandPass:
		case filterType_notch:
			// Quality : USE IT
			// Gain : Not USE IT
			break;
		case filterType_peak:
			// Quality : USE IT
			// Gain : USE IT
			break;
		case filterType_lowShelf:
		case filterType_highShelf:
			// Quality : NOT USE IT
			// Gain : USE IT
			break;
		default:
			// Quality : USE IT
			// Gain : USE IT
			break;
	}
	double norm;
	double V = std::pow(10.0, std::abs(m_gain) / 20.0);
	double K = std::tan(M_PI * m_frequencyCut / getOutputFormat().getFrequency());
	switch (m_type) {
		case filterType_none:
			m_a[0] = 1.0;
			m_a[1] = 0.0;
			m_a[2] = 0.0;
			m_b[0] = 0.0;
			m_b[1] = 0.0;
			break;
		case filterType_lowPass:
			norm = 1 / (1 + K / m_qualityFactor + K * K);
			m_a[0] = K * K * norm;
			m_a[1] = 2 * m_a[0];
			m_a[2] = m_a[0];
			m_b[0] = 2 * (K * K - 1) * norm;
			m_b[1] = (1 - K / m_qualityFactor + K * K) * norm;
			break;
		case filterType_highPass:
			norm = 1 / (1 + K / m_qualityFactor + K * K);
			m_a[0] = 1 * norm;
			m_a[1] = -2 * m_a[0];
			m_a[2] = m_a[0];
			m_b[0] = 2 * (K * K - 1) * norm;
			m_b[1] = (1 - K / m_qualityFactor + K * K) * norm;
			break;
		case filterType_bandPass:
			norm = 1 / (1 + K / m_qualityFactor + K * K);
			m_a[0] = K / m_qualityFactor * norm;
			m_a[1] = 0;
			m_a[2] = -m_a[0];
			m_b[0] = 2 * (K * K - 1) * norm;
			m_b[1] = (1 - K / m_qualityFactor + K * K) * norm;
			break;
		case filterType_notch:
			norm = 1 / (1 + K / m_qualityFactor + K * K);
			m_a[0] = (1 + K * K) * norm;
			m_a[1] = 2 * (K * K - 1) * norm;
			m_a[2] = m_a[0];
			m_b[0] = m_a[1];
			m_b[1] = (1 - K / m_qualityFactor + K * K) * norm;
			break;
		case filterType_peak:
			if (m_gain >= 0) {
				norm = 1 / (1 + 1/m_qualityFactor * K + K * K);
				m_a[0] = (1 + V/m_qualityFactor * K + K * K) * norm;
				m_a[1] = 2 * (K * K - 1) * norm;
				m_a[2] = (1 - V/m_qualityFactor * K + K * K) * norm;
				m_b[0] = m_a[1];
				m_b[1] = (1 - 1/m_qualityFactor * K + K * K) * norm;
			} else {
				norm = 1 / (1 + V/m_qualityFactor * K + K * K);
				m_a[0] = (1 + 1/m_qualityFactor * K + K * K) * norm;
				m_a[1] = 2 * (K * K - 1) * norm;
				m_a[2] = (1 - 1/m_qualityFactor * K + K * K) * norm;
				m_b[0] = m_a[1];
				m_b[1] = (1 - V/m_qualityFactor * K + K * K) * norm;
			}
			break;
		case filterType_lowShelf:
			if (m_gain >= 0) {
				norm = 1 / (1 + M_SQRT2 * K + K * K);
				m_a[0] = (1 + std::sqrt(2*V) * K + V * K * K) * norm;
				m_a[1] = 2 * (V * K * K - 1) * norm;
				m_a[2] = (1 - std::sqrt(2*V) * K + V * K * K) * norm;
				m_b[0] = 2 * (K * K - 1) * norm;
				m_b[1] = (1 - M_SQRT2 * K + K * K) * norm;
			} else {
				norm = 1 / (1 + std::sqrt(2*V) * K + V * K * K);
				m_a[0] = (1 + M_SQRT2 * K + K * K) * norm;
				m_a[1] = 2 * (K * K - 1) * norm;
				m_a[2] = (1 - M_SQRT2 * K + K * K) * norm;
				m_b[0] = 2 * (V * K * K - 1) * norm;
				m_b[1] = (1 - std::sqrt(2*V) * K + V * K * K) * norm;
			}
			break;
		case filterType_highShelf:
			if (m_gain >= 0) {
				norm = 1 / (1 + M_SQRT2 * K + K * K);
				m_a[0] = (V + std::sqrt(2*V) * K + K * K) * norm;
				m_a[1] = 2 * (K * K - V) * norm;
				m_a[2] = (V - std::sqrt(2*V) * K + K * K) * norm;
				m_b[0] = 2 * (K * K - 1) * norm;
				m_b[1] = (1 - M_SQRT2 * K + K * K) * norm;
			} else {
				norm = 1 / (V + std::sqrt(2*V) * K + K * K);
				m_a[0] = (1 + M_SQRT2 * K + K * K) * norm;
				m_a[1] = 2 * (K * K - 1) * norm;
				m_a[2] = (1 - M_SQRT2 * K + K * K) * norm;
				m_b[0] = 2 * (K * K - V) * norm;
				m_b[1] = (V - std::sqrt(2*V) * K + K * K) * norm;
			}
			break;
	}
}
