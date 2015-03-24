/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __DRAIN_ALGO_RESAMPLER_H__
#define __DRAIN_ALGO_RESAMPLER_H__

#include <drain/Algo.h>
#ifdef HAVE_SPEEX_DSP_RESAMPLE
	#include <speex/speex_resampler.h>
#endif
#include <etk/memory.h>

namespace drain {
	// TODO: Manage change timestamp when pull mode
	// TODO: drain ...
	class Resampler : public Algo {
		private:
			#ifdef HAVE_SPEEX_DSP_RESAMPLE
				SpeexResamplerState* m_speexResampler;
			#endif
			size_t m_positionRead; //!< For residual data in the buffer last read number of chunk
			size_t m_positionWrite; //!< Current pointer of writing new output data of resampler
		protected:
			/**
			 * @brief Constructor
			 */
			Resampler();
			void init();
		public:
			static std11::shared_ptr<Resampler> create();
			/**
			 * @brief Destructor
			 */
			virtual ~Resampler();
		protected:
			virtual void configurationChange();
		public:
			virtual bool process(std11::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
		private:
			std11::chrono::nanoseconds m_residualTimeInResampler; //!< the time of data locked in the resampler ...
	};
};

#endif
