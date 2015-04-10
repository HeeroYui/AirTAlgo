/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/drain/debug.h>
#include <audio/drain/BiQuadFloat.h>

static const char* listValues[] = {
	"none",
	"low-pass",
	"high-pass",
	"band-pass",
	"notch",
	"peak",
	"low-shelf",
	"high-shelf"
};
static int32_t listValuesSize = sizeof(listValues)/sizeof(char*);


namespace etk {
	template<> std::string to_string<enum audio::drain::filterType>(const enum audio::drain::filterType& _variable) {
		return listValues[_variable];
	}
	template <> bool from_string<enum audio::drain::filterType>(enum audio::drain::filterType& _variableRet, const std::string& _value) {
		for (int32_t iii=0; iii<listValuesSize; ++iii) {
			if (_value == listValues[iii]) {
				_variableRet = static_cast<enum audio::drain::filterType>(iii);
				return true;
			}
		}
		_variableRet = audio::drain::filterType_none;
		return false;
	}
}



audio::drain::BiQuadFloat::BiQuadFloat() {
	reset();
	// reset coefficients
	m_a[0] = 1.0;
	m_a[1] = 0.0;
	m_a[2] = 0.0;
	m_b[0] = 0.0;
	m_b[1] = 0.0;
}


float audio::drain::BiQuadFloat::processFloat(float _sample) {
	float result;
	// compute
	result =   m_a[0] * _sample
	         + m_a[1] * m_x[0]
	         + m_a[2] * m_x[1]
	         - m_b[0] * m_y[0]
	         - m_b[1] * m_y[1];
	//update history of X
	m_x[1] = m_x[0];
	m_x[0] = _sample;
	//update history of Y
	m_y[1] = m_y[0];
	m_y[0] = result;
	return result;
}

void audio::drain::BiQuadFloat::processFloat(float* _input,
                                             float* _output,
                                             size_t _nbChunk,
                                             int32_t _inputOffset,
                                             int32_t _outputOffset) {
	for (size_t iii=0; iii<_nbChunk; ++iii) {
		// process in float the biquad.
		*_output = processFloat(*_input);
		// move to the sample on the same channel.
		_input += _inputOffset;
		_output += _outputOffset;
	}
}

void audio::drain::BiQuadFloat::processInt16(int16_t* _input,
                                             int16_t* _output,
                                             size_t _nbChunk,
                                             int32_t _inputOffset,
                                             int32_t _outputOffset) {
	for (size_t iii=0; iii<_nbChunk; ++iii) {
		// process in float the biquad.
		float out = processFloat(*_input);
		// Limit output.
		out = std::avg(-32768.0f, out, 32767.0f);
		*_output = static_cast<int16_t>(out);
		// move to the sample on the same channel.
		_input += _inputOffset;
		_output += _outputOffset;
	}
}


void audio::drain::BiQuadFloat::setBiquad(enum audio::drain::filterType _type, double _frequencyCut, double _qualityFactor, double _gain, float _sampleRate) {
	reset();
	if (_sampleRate < 1) {
		m_a[0] = 1.0;
		m_a[1] = 0.0;
		m_a[2] = 0.0;
		m_b[0] = 0.0;
		m_b[1] = 0.0;
		return;
	}
	if (_frequencyCut > _sampleRate/2) {
		_frequencyCut = _sampleRate/2;
	} else if (_frequencyCut < 0) {
		_frequencyCut = 0;
	}
	if (_qualityFactor < 0.01) {
		_qualityFactor = 0.01;
	}
	double norm;
	double V = std::pow(10.0, std::abs(_gain) / 20.0);
	double K = std::tan(M_PI * _frequencyCut / _sampleRate);
	switch (_type) {
		case filterType_none:
			m_a[0] = 1.0;
			m_a[1] = 0.0;
			m_a[2] = 0.0;
			m_b[0] = 0.0;
			m_b[1] = 0.0;
			break;
		case filterType_lowPass:
			norm = 1 / (1 + K / _qualityFactor + K * K);
			m_a[0] = K * K * norm;
			m_a[1] = 2 * m_a[0];
			m_a[2] = m_a[0];
			m_b[0] = 2 * (K * K - 1) * norm;
			m_b[1] = (1 - K / _qualityFactor + K * K) * norm;
			break;
		case filterType_highPass:
			norm = 1 / (1 + K / _qualityFactor + K * K);
			m_a[0] = 1 * norm;
			m_a[1] = -2 * m_a[0];
			m_a[2] = m_a[0];
			m_b[0] = 2 * (K * K - 1) * norm;
			m_b[1] = (1 - K / _qualityFactor + K * K) * norm;
			break;
		case filterType_bandPass:
			norm = 1 / (1 + K / _qualityFactor + K * K);
			m_a[0] = K / _qualityFactor * norm;
			m_a[1] = 0;
			m_a[2] = -m_a[0];
			m_b[0] = 2 * (K * K - 1) * norm;
			m_b[1] = (1 - K / _qualityFactor + K * K) * norm;
			break;
		case filterType_notch:
			norm = 1 / (1 + K / _qualityFactor + K * K);
			m_a[0] = (1 + K * K) * norm;
			m_a[1] = 2 * (K * K - 1) * norm;
			m_a[2] = m_a[0];
			m_b[0] = m_a[1];
			m_b[1] = (1 - K / _qualityFactor + K * K) * norm;
			break;
		case filterType_peak:
			if (_gain >= 0) {
				norm = 1 / (1 + 1/_qualityFactor * K + K * K);
				m_a[0] = (1 + V/_qualityFactor * K + K * K) * norm;
				m_a[1] = 2 * (K * K - 1) * norm;
				m_a[2] = (1 - V/_qualityFactor * K + K * K) * norm;
				m_b[0] = m_a[1];
				m_b[1] = (1 - 1/_qualityFactor * K + K * K) * norm;
			} else {
				norm = 1 / (1 + V/_qualityFactor * K + K * K);
				m_a[0] = (1 + 1/_qualityFactor * K + K * K) * norm;
				m_a[1] = 2 * (K * K - 1) * norm;
				m_a[2] = (1 - 1/_qualityFactor * K + K * K) * norm;
				m_b[0] = m_a[1];
				m_b[1] = (1 - V/_qualityFactor * K + K * K) * norm;
			}
			break;
		case filterType_lowShelf:
			if (_gain >= 0) {
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
			if (_gain >= 0) {
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

void audio::drain::BiQuadFloat::reset() {
	m_x[0] = 0;
	m_y[1] = 0;
	m_x[0] = 0;
	m_y[1] = 0;
}

void audio::drain::BiQuadFloat::setBiquadCoef(float _a0, float _a1, float _a2, float _b0, float _b1) {
	m_a[0] = _a0;
	m_a[1] = _a1;
	m_a[2] = _a2;
	m_b[0] = _b0;
	m_b[1] = _b1;
	reset();
}
void audio::drain::BiQuadFloat::getBiquadCoef(float& _a0, float& _a1, float& _a2, float& _b0, float& _b1) {
	_a0 = m_a[0];
	_a1 = m_a[1];
	_a2 = m_a[2];
	_b0 = m_b[0];
	_b1 = m_b[1];
}

std::vector<float> audio::drain::BiQuadFloat::getCoef() {
	std::vector<float> out;
	out.push_back(m_a[0]);
	out.push_back(m_a[1]);
	out.push_back(m_a[2]);
	out.push_back(m_b[0]);
	out.push_back(m_b[1]);
	return out;
}


std::vector<std::pair<float,float> > audio::drain::BiQuadFloat::calculateTheory(double _sampleRate) {
	std::vector<std::pair<float,float> > out;
	double norm;
	
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
		
		//APPL_DEBUG("theory = " << freq << " power=" << y);
		out.push_back(std::make_pair<float,float>(freq, y));
	}
	return out;
}
