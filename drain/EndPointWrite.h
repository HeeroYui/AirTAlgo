/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_ALGO_END_POINT_WRITE_H__
#define __AIRT_ALGO_ALGO_END_POINT_WRITE_H__

#include <drain/EndPoint.h>
#include <mutex>
#include <functional>

namespace drain{
	typedef std::function<void (const std::chrono::system_clock::time_point& _playTime,
	                            const size_t& _nbChunk,
	                            const std::vector<audio::channel>& _map,
	                            enum audio::format _type)> needDataFunctionWrite;
	class EndPointWrite : public EndPoint {
		private:
			std::vector<int8_t> m_tmpData;
			needDataFunctionWrite m_function;
			std::mutex m_mutex;
		protected:
			/**
			 * @brief Constructor
			 */
			EndPointWrite();
			void init();
		public:
			static std::shared_ptr<EndPointWrite> create();
			/**
			 * @brief Destructor
			 */
			virtual ~EndPointWrite() {};
			virtual void configurationChange();
			virtual bool process(std::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
			virtual void write(const void* _value, size_t _nbChunk);
			virtual void setCallback(needDataFunctionWrite _function) {
				m_function = _function;
			}
	};
};

#endif
