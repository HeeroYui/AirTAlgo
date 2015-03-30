/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __DRAIN_ALGO_EQUALIZER_H__
#define __DRAIN_ALGO_EQUALIZER_H__

#include <drain/Algo.h>
#include <etk/memory.h>
#include <ejson/Object.h>
#include <drain/BiQuadFloat.h>

namespace drain {
	class Equalizer : public Algo {
		protected:
			/**
			 * @brief Constructor
			 */
			Equalizer();
			void init();
		public:
			static std11::shared_ptr<Equalizer> create();
			/**
			 * @brief Destructor
			 */
			virtual ~Equalizer();
		protected:
			virtual void configurationChange();
		public:
			virtual bool process(std11::chrono::system_clock::time_point& _time,
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
			/**
			 * @brief repesent all the biquad to process:
			 * The first vector represent the number of channel to process
			 * The second vector represent the number of biquad to process
			 */
			std::vector<std::vector<BiQuadFloat> > m_biquads;
			/**
			 * @brief Configure biquad with the  user spec.
			 */
			void configureBiQuad();
	};
};

#endif
