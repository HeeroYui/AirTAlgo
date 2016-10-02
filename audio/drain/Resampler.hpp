/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <audio/drain/Algo.hpp>
#ifdef HAVE_SPEEX_DSP_RESAMPLE
	#include <speex/speex_resampler.h>
#endif
#include <ememory/memory.hpp>

namespace audio {
	namespace drain {
		// TODO: Manage change timestamp when pull mode
		// TODO: drain ...
		class Resampler : public audio::drain::Algo {
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
				static ememory::SharedPtr<audio::drain::Resampler> create();
				/**
				 * @brief Destructor
				 */
				virtual ~Resampler();
			protected:
				virtual void configurationChange();
			public:
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
			private:
				audio::Duration m_residualTimeInResampler; //!< the time of data locked in the resampler ...
		};
	}
}

