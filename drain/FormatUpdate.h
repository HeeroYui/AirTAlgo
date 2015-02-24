/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#ifndef __AIRT_ALGO_FORMAT_UPDATE_H__
#define __AIRT_ALGO_FORMAT_UPDATE_H__

#include <drain/Algo.h>

namespace drain {
	class FormatUpdate : public Algo {
		protected:
			/**
			 * @brief Constructor
			 */
			FormatUpdate();
			void init();
		public:
			static std11::shared_ptr<FormatUpdate> create();
			/**
			 * @brief Destructor
			 */
			virtual ~FormatUpdate() {};
		protected:
			virtual void configurationChange();
		public:
			virtual bool process(std11::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
		private:
			void (*m_functionConvert)(void* _input, void* _output, size_t _nbSample);
	};
};

#endif
