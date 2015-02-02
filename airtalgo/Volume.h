/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_VOLUME_H__
#define __AIRT_ALGO_VOLUME_H__

#include <airtalgo/Algo.h>
#ifdef HAVE_SPEEX_DSP_RESAMPLE
	#include <speex/speex_resampler.h>
#endif
#include <memory>

namespace airtalgo {
	// TODO: Optimisation
	// TODO: Zero crossing
	// TODO: Continuous update volume
	// TODO: Manage multiple volume
	// TODO: Manage set volume
	class Volume : public Algo {
		private:
			float m_volumedB;
			// for float input :
			float m_volumeAppli;
			// for integer input :
			int32_t m_volumeDecalage; // Volume to apply is simple as : X * m_coef >> m_coef
			int32_t m_volumeCoef;
			// convertion function:
			void (*m_functionConvert)(void* _input, void* _output, size_t _nbSample, int32_t _volumeCoef, int32_t _volumeDecalage, float _volumeAppli);
		protected:
			/**
			 * @brief Constructor
			 */
			Volume();
			void init();
		public:
			static std::shared_ptr<Volume> create();
			/**
			 * @brief Destructor
			 */
			virtual ~Volume();
		protected:
			virtual void configurationChange();
		public:
			virtual bool process(std::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
		public:
			virtual std::vector<airtalgo::format> getFormatSupportedInput();
			virtual std::vector<airtalgo::format> getFormatSupportedOutput();
		protected:
			virtual void updateVolumeValues();
	};
};

#endif
