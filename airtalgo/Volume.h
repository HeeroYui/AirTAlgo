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
	class Volume : public Algo {
		private:
			float m_volumedB;
			float m_volumeAppli;
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
	};
};

#endif
