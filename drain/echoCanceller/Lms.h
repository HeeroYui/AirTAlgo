/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __DRAIN_LMS_H__
#define __DRAIN_LMS_H__

#include <etk/types.h>
#include <etk/chrono.h>

namespace drain {
	// Least Mean Square (LMS) algorithm "echo canceller"
	class Lms {
		public:
			Lms(void);
			~Lms(void);
		public:
			void reset(void);
			bool process(int16_t* _output, int16_t* _feedback, int16_t* _microphone, int32_t _nbSample);
			bool process(float* _output, float* _feedback, float* _microphone, int32_t _nbSample);
		protected:
			float processValue(float* _feedback, float _microphone);
		private:
			std::vector<float> m_filtre;
			std::vector<float> m_feedBack;
		public:
			void setFilterSize(size_t _sampleRate, std11::chrono::microseconds _time);
			void setFilterSize(size_t _nbSample);
		protected:
			float m_micro; //!< µ step size
	};
}

#endif
