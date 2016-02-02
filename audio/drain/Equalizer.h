/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <audio/drain/Algo.h>
#include <memory>
#include <ejson/Object.h>
#include <audio/algo/drain/Equalizer.h>

namespace audio {
	namespace drain {
		class Equalizer : public Algo {
			protected:
				/**
				 * @brief Constructor
				 */
				Equalizer();
				void init();
			public:
				static std11::shared_ptr<audio::drain::Equalizer> create();
				/**
				 * @brief Destructor
				 */
				virtual ~Equalizer();
			protected:
				virtual void configurationChange();
			public:
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
			protected:
				std11::shared_ptr<ejson::Object> m_config; // configuration of the equalizer.
			public:
				virtual bool setParameter(const std::string& _parameter, const std::string& _value);
				virtual std::string getParameter(const std::string& _parameter) const;
				virtual std::string getParameterProperty(const std::string& _parameter) const;
			protected:
				void addBiquad(int32_t _idBiquad, const std11::shared_ptr<const ejson::Object>& _object);
				audio::algo::drain::Equalizer m_algo;
				/**
				 * @brief Configure biquad with the  user spec.
				 */
				void configureBiQuad();
			public:
				// for debug & tools only
				std::vector<std::pair<float,float> > calculateTheory();
		};
	}
}

