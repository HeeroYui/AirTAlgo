/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <audio/drain/Equalizer.hpp>
#include <audio/drain/Algo.hpp>
#include <audio/drain/debug.hpp>

// see http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
// see http://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/

audio::drain::Equalizer::Equalizer() {
	
}

void audio::drain::Equalizer::init() {
	audio::drain::Algo::init();
	audio::drain::Algo::m_type = "Equalizer";
	m_supportedFormat.push_back(audio::format_int16);
	m_supportedFormat.push_back(audio::format_int16);
	configureBiQuad();
}

ememory::SharedPtr<audio::drain::Equalizer> audio::drain::Equalizer::create() {
	ememory::SharedPtr<audio::drain::Equalizer> tmp(new audio::drain::Equalizer());
	tmp->init();
	return tmp;
}

audio::drain::Equalizer::~Equalizer() {
	
}

void audio::drain::Equalizer::configurationChange() {
	audio::drain::Algo::configurationChange();
	configureBiQuad();
}

bool audio::drain::Equalizer::process(audio::Time& _time,
                                      void* _input,
                                      size_t _inputNbChunk,
                                      void*& _output,
                                      size_t& _outputNbChunk) {
	_outputNbChunk = _inputNbChunk;
	_output = _input;
	if (_input == nullptr) {
		return false;
	}
	m_algo.process(_output, _input, _inputNbChunk);
	return true;
}

bool audio::drain::Equalizer::setParameter(const std::string& _parameter, const std::string& _value) {
	//DRAIN_WARNING("set : " << _parameter << " " << _value);
	if (_parameter == "config") {
		m_config = ejson::Object(_value);
		configureBiQuad();
	} else if (_parameter == "reset") {
		// TODO : m_algo.reset();
		return true;
	}
	return false;
}

std::string audio::drain::Equalizer::getParameter(const std::string& _parameter) const {
	return "error";
}

std::string audio::drain::Equalizer::getParameterProperty(const std::string& _parameter) const {
	return "error";
}

void audio::drain::Equalizer::addBiquad(int32_t _idBiquad, const ejson::Object& _object) {
	// get type:
	std::string typeString = _object["type"].toString().get("none");
	if (typeString == "direct-value") {
		double a0 = _object["a0"].toNumber().get(0.0);
		double a1 = _object["a1"].toNumber().get(0.0);
		double a2 = _object["a2"].toNumber().get(0.0);
		double b0 = _object["b0"].toNumber().get(0.0);
		double b1 = _object["b1"].toNumber().get(0.0);
		if (_idBiquad < 0) {
			m_algo.addBiquad(a0, a1, a2, b0, b1);
		} else {
			m_algo.addBiquad(_idBiquad, a0, a1, a2, b0, b1);
		}
	} else {
		enum audio::algo::drain::biQuadType type;
		if (etk::from_string(type, typeString) == false) {
			DRAIN_ERROR("Can not parse equalizer type:'" << typeString << "'");
		}
		double gain = _object["gain"].toNumber().get(0.0);
		double frequency = _object["cut-frequency"].toNumber().get(0.0);
		double quality = _object["quality"].toNumber().get(0.0);
		if (_idBiquad < 0) {
			m_algo.addBiquad(type, frequency, quality, gain);
		} else {
			m_algo.addBiquad(_idBiquad, type, frequency, quality, gain);
		}
	}
}

void audio::drain::Equalizer::configureBiQuad() {
	m_algo.init(getOutputFormat().getFrequency(),
	            getOutputFormat().getMap().size(),
	            getOutputFormat().getFormat());
	if (m_config.exist() == false) {
		return;
	}
	// check for a global config:
	const ejson::Array global = m_config["global"].toArray();
	if (global.exist() == true) {
		// only global configuration get all elements:
		for (size_t kkk=0; kkk<global.size(); ++kkk) {
			const ejson::Object tmpObject = global[kkk].toObject();
			if (tmpObject.exist() == false) {
				DRAIN_ERROR("Parse the configuration error : not a correct parameter:" << kkk);
				continue;
			}
			// Add biquad...
			addBiquad(-1, tmpObject);
		}
		return;
	}
	for (size_t iii=0; iii<getOutputFormat().getMap().size(); ++iii) {
		std::string channelName = etk::to_string(getOutputFormat().getMap()[iii]);
		const ejson::Array channelConfig = m_config[channelName].toArray();
		if (channelConfig.exist() == false) {
			// no config ... not a problem ...
			continue;
		}
		// only global configuration get all elements:
		for (size_t kkk=0; kkk<channelConfig.size(); ++kkk) {
			const ejson::Object tmpObject = channelConfig[kkk].toObject();
			if (tmpObject.exist() == false) {
				DRAIN_ERROR("Parse the configuration error : not a correct parameter:" << kkk);
				continue;
			}
			// add biquad:
			addBiquad(kkk, tmpObject);
		}
	}
	return;
}

std::vector<std::pair<float,float> > audio::drain::Equalizer::calculateTheory() {
	return m_algo.calculateTheory();
}