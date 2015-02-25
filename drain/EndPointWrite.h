/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_ALGO_END_POINT_WRITE_H__
#define __AIRT_ALGO_ALGO_END_POINT_WRITE_H__

#include <drain/EndPoint.h>
#include <etk/functional.h>
#include <etk/mutex.h>

namespace drain{
	typedef std11::function<void (const std11::chrono::system_clock::time_point& _time,
	                              size_t _nbChunk,
	                              enum audio::format _format,
	                              uint32_t _frequency,
	                              const std::vector<audio::channel>& _map)> playbackFunctionWrite;
	class EndPointWrite : public EndPoint {
		private:
			std::vector<int8_t> m_tmpData;
			playbackFunctionWrite m_function;
			std11::mutex m_mutex;
		protected:
			/**
			 * @brief Constructor
			 */
			EndPointWrite();
			void init();
		public:
			static std11::shared_ptr<EndPointWrite> create();
			/**
			 * @brief Destructor
			 */
			virtual ~EndPointWrite() {};
			virtual void configurationChange();
			virtual bool process(std11::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
			virtual void write(const void* _value, size_t _nbChunk);
			virtual void setCallback(playbackFunctionWrite _function) {
				m_function = _function;
			}
	};
};

#endif
