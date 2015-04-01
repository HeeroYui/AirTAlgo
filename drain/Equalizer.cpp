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
	configureBiQuad();
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
	if (m_biquads.size() != getOutputFormat().getMap().size()) {
		configureBiQuad();
	}
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
			for (size_t iii=0; iii<m_biquads[jjj].size(); ++iii) {
				m_biquads[jjj][iii].processFloat(data, data, _inputNbChunk, getInputFormat().getMap().size(), getOutputFormat().getMap().size() );
			}
		}
	} else if (getOutputFormat().getFormat() == audio::format_int16) {
		for (size_t jjj=0; jjj<getOutputFormat().getMap().size(); ++jjj) {
			// get pointer on data:
			int16_t* data = static_cast<int16_t*>(_input);
			// move to sample offset:
			data += jjj;
			for (size_t iii=0; iii<m_biquads[jjj].size(); ++iii) {
				m_biquads[jjj][iii].processInt16(data, data, _inputNbChunk, getInputFormat().getMap().size(), getOutputFormat().getMap().size() );
			}
		}
	}
	return true;
}

bool drain::Equalizer::setParameter(const std::string& _parameter, const std::string& _value) {
	//DRAIN_WARNING("set : " << _parameter << " " << _value);
	if (_parameter == "config") {
		m_config = ejson::Object::create(_value);
		configureBiQuad();
	} else if (_parameter == "reset") {
		for (int32_t iii=0; iii<m_biquads.size(); ++iii) {
			for (int32_t jjj=0; jjj<m_biquads[iii].size(); ++jjj) {
				m_biquads[iii][jjj].reset();
			}
		}
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

static drain::BiQuadFloat getBiquad(const std11::shared_ptr<const ejson::Object>& _object, float _frequency) {
	drain::BiQuadFloat out;
	// get type:
	std::string typeString = _object->getStringValue("type", "none");
	if (typeString == "direct-value") {
		double a0 = _object->getNumberValue("a0", 0.0);
		double a1 = _object->getNumberValue("a1", 0.0);
		double a2 = _object->getNumberValue("a2", 0.0);
		double b0 = _object->getNumberValue("b0", 0.0);
		double b1 = _object->getNumberValue("b1", 0.0);
		out.setBiquadCoef(a0, a1, a2, b0, b1);
	} else {
		enum drain::filterType type;
		if (etk::from_string(type, typeString) == false) {
			DRAIN_ERROR("Can not parse equalizer type:'" << typeString << "'");
		}
		double gain = _object->getNumberValue("gain", 0.0);
		double frequency = _object->getNumberValue("cut-frequency", 0.0);
		double quality = _object->getNumberValue("quality", 0.0);
		out.setBiquad(type, frequency, quality, gain, _frequency);
	}
	return out;
}

void drain::Equalizer::configureBiQuad() {
	m_biquads.clear();
	m_biquads.resize(getOutputFormat().getMap().size());
	if (m_config == nullptr) {
		return;
	}
	// check for a global config:
	const std11::shared_ptr<const ejson::Array> global = m_config->getArray("global");
	if (global != nullptr) {
		// only global configuration get all elements:
		for (size_t kkk=0; kkk<global->size(); ++kkk) {
			const std11::shared_ptr<const ejson::Object> tmpObject = global->getObject(kkk);
			if (tmpObject == nullptr) {
				DRAIN_ERROR("Parse the configuration error : not a correct parameter:" << kkk);
				continue;
			}
			// declare biquad:
			drain::BiQuadFloat biquad = getBiquad(tmpObject, getOutputFormat().getFrequency());
			// add this bequad for every Channel:
			for (size_t iii=0; iii<m_biquads.size(); ++iii) {
				m_biquads[iii].push_back(biquad);
			}
		}
		return;
	}
	for (size_t iii=0; iii<getOutputFormat().getMap().size(); ++iii) {
		std::string channelName = etk::to_string(getOutputFormat().getMap()[iii]);
		const std11::shared_ptr<const ejson::Array> channelConfig = m_config->getArray(channelName);
		
		if (channelConfig == nullptr) {
			// no config ... not a problem ...
			continue;
		}
		// only global configuration get all elements:
		for (size_t kkk=0; kkk<channelConfig->size(); ++kkk) {
			const std11::shared_ptr<const ejson::Object> tmpObject = channelConfig->getObject(kkk);
			if (tmpObject == nullptr) {
				DRAIN_ERROR("Parse the configuration error : not a correct parameter:" << kkk);
				continue;
			}
			// declare biquad:
			drain::BiQuadFloat biquad = getBiquad(tmpObject, getOutputFormat().getFrequency());
			// add this bequad for specific channel:
			m_biquads[iii].push_back(biquad);
		}
	}
	return;
}

std::vector<std::pair<float,float> > drain::Equalizer::calculateTheory() {
	std::vector<std::pair<float,float> > out;
	for (size_t iii=0; iii<m_biquads[0].size(); ++iii) {
		if (iii == 0) {
			out = m_biquads[0][iii].calculateTheory(getOutputFormat().getFrequency());
		} else {
			std::vector<std::pair<float,float> > tmp = m_biquads[0][iii].calculateTheory(getOutputFormat().getFrequency());
			for (size_t jjj=0; jjj< out.size(); ++jjj) {
				out[jjj].second += tmp[jjj].second;
			}
		}
	}
	return out;
}