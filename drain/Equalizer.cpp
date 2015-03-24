/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <drain/debug.h>
#include <drain/Equalizer.h>
#include <drain/Algo.h>

// see http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

drain::Equalizer::Equalizer() {
	
}

void drain::Equalizer::init() {
	drain::Algo::init();
	drain::Algo::m_type = "Equalizer";
	m_supportedFormat.push_back(audio::format_int16);
	m_type = drain::filterType_none;
	m_gain = 0;
	m_frequency = 1000;
	m_bandWidth = 200;
	configureBiQuad();
	// reset coefficients
	m_b[0] = 1.0;
	m_b[1] = 0.0;
	m_b[2] = 0.0;
	m_a[0] = 0.0;
	m_a[1] = 0.0;
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
	return true;
}

bool drain::Equalizer::setParameter(const std::string& _parameter, const std::string& _value) {
	if (_parameter == "type") {
		if (_value == "none") {
			m_type = drain::filterType_none;
		} else if (_value == "LPF") {
			m_type = drain::filterType_LPF;
		} else if (_value == "HPF") {
			m_type = drain::filterType_HPF;
		} else if (_value == "BPF") {
			m_type = drain::filterType_BPF;
		} else if (_value == "NOTCH") {
			m_type = drain::filterType_NOTCH;
		} else if (_value == "APF") {
			m_type = drain::filterType_APF;
		} else if (_value == "PeakingEQ") {
			m_type = drain::filterType_PeakingEQ;
		} else if (_value == "LSH") {
			m_type = drain::filterType_LSH;
		} else if (_value == "HSH") {
			m_type = drain::filterType_HSH;
		} else if (_value == "EQU"){
			m_type = drain::filterType_EQU;
		} else {
			DRAIN_ERROR("Can not set equalizer type : " << _value);
			return false;
		}
		configureBiQuad();
		return true;
	} else if (_parameter == "gain") {
		m_gain = etk::string_to_int32_t(_value);
		configureBiQuad();
		return true;
	} else if (_parameter == "frequency") {
		m_frequency = etk::string_to_int32_t(_value);
		configureBiQuad();
		return true;
	} else if (_parameter == "band-width") {
		m_bandWidth = etk::string_to_int32_t(_value);
		configureBiQuad();
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
	result =   m_b[0] * _sample
	         + m_b[1] * _history.m_x[0]
	         + m_b[2] * _history.m_x[1]
	         - m_a[0] * _history.m_y[0]
	         - m_a[1] * _history.m_y[1];
	//update history of X
	_history.m_x[0] = _history.m_x[1];
	_history.m_x[1] = _sample;
	//update history of Y
	_history.m_y[0] = _history.m_y[1];
	_history.m_y[1] = result;
	return result;
}

bool drain::Equalizer::configureBiQuad() {
	// reset biQuad.
	m_b[0] = 1.0;
	m_b[1] = 0.0;
	m_b[2] = 0.0;
	m_a[0] = 0.0;
	m_a[1] = 0.0;
	if (m_type == filterType_none) {
		return true;
	}
	double a0, a1, a2, b0, b1, b2;
	/* setup variables */
	double A = std::pow(10, m_gain /40); // used for peaking and shelving EQ filters only
	double w0 = 2.0 * M_PI * double(m_frequency) / double(getOutputFormat().getFrequency());
	
	// 2*sqrt(A)*alpha  =  sin(w0) * sqrt( (A^2 + 1)*(1/S - 1) + 2*A )
	//    is a handy intermediate variable for shelving EQ filters.
	double alpha = std::sin(w0) * std::sqrt((A*A+1.0)*(1.0/m_bandWidth - 1.0) + 2*A);
	alpha /= 2.0*std::sqrt(A);
	
	
	switch (m_type) {
		case drain::filterType_LPF:
			b0 = (1.0 - std::cos(w0)) * 0.5;
			b1 = 1.0 - std::cos(w0);
			b2 = (1.0 - std::cos(w0)) * 0.5;
			a0 = 1.0 + alpha;
			a1 = -2.0 * std::cos(w0);
			a2 = 1.0 - alpha;
			break;
		case drain::filterType_HPF:
			b0 = (1.0 + std::cos(w0)) * 0.5;
			b1 = -(1.0 + std::cos(w0));
			b2 = (1.0 + std::cos(w0)) * 0.5;
			a0 = 1.0 + alpha;
			a1 = -2.0 * std::cos(w0);
			a2 = 1.0 - alpha;
			break;
		case drain::filterType_BPF: // constant 0dB peak gain
			b0 = alpha;
			b1 = 0.0;
			b2 = -alpha;
			a0 = 1.0 + alpha;
			a1 = -2.0 * std::cos(w0);
			a2 = 1.0 - alpha;
			break;
		case drain::filterType_NOTCH:
			b0 = 1.0;
			b1 = -2.0 * std::cos(w0);
			b2 = 1.0;
			a0 = 1.0 + alpha;
			a1 = -2.0 * std::cos(w0);
			a2 = 1.0 - alpha;
			break;
		case drain::filterType_APF:
			b0 = 1.0 - alpha;
			b1 = -2.0 * std::cos(w0);
			b2 = 1.0 + alpha;
			a0 = 1.0 + alpha;
			a1 = -2.0 * std::cos(w0);
			a2 = 1.0 - alpha;
			break;
		case drain::filterType_PeakingEQ:
			b0 = 1.0 + (alpha * A);
			b1 = -2.0 * std::cos(w0);
			b2 = 1.0 - (alpha * A);
			a0 = 1.0 + (alpha /A);
			a1 = -2.0 * std::cos(w0);
			a2 = 1.0 - (alpha /A);
			break;
		case drain::filterType_LSH:
			b0 =     A*( (A+1.0) - (A-1.0)*std::cos(w0) + 2.0*std::sqrt(A)*alpha );
			b1 = 2.0*A*( (A-1.0) - (A+1.0)*std::cos(w0)                        );
			b2 =     A*( (A+1.0) - (A-1.0)*std::cos(w0) - 2.0*std::sqrt(A)*alpha );
			a0 =         (A+1.0) + (A-1.0)*std::cos(w0) + 2.0*std::sqrt(A)*alpha;
			a1 =  -2.0*( (A-1.0) + (A+1.0)*std::cos(w0)                        );
			a2 =         (A+1.0) + (A-1.0)*std::cos(w0) - 2.0*std::sqrt(A)*alpha;
			break;
		case drain::filterType_HSH:
			b0 =      A*( (A+1.0) + (A-1.0) * std::cos(w0) + 2.0*std::sqrt(A)*alpha );
			b1 = -2.0*A*( (A-1.0) + (A+1.0) * std::cos(w0)                        );
			b2 =      A*( (A+1.0) + (A-1.0) * std::cos(w0) - 2.0*std::sqrt(A)*alpha );
			a0 =          (A+1.0) - (A-1.0) * std::cos(w0) + 2.0*std::sqrt(A)*alpha;
			a1 =    2.0*( (A-1.0) - (A+1.0) * std::cos(w0)                        );
			a2 =          (A+1.0) - (A-1.0) * std::cos(w0) - 2.0*std::sqrt(A)*alpha;
			break;
		default:
			DRAIN_CRITICAL("Impossible case ...");
			return false;
	}
	// precalculate coefficients:
	m_b[0] = b0 /a0;
	m_b[1] = b1 /a0;
	m_b[2] = b2 /a0;
	m_a[0] = a1 /a0;
	m_a[1] = a2 /a0;
	return true;
}

