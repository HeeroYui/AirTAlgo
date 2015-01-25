/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#ifndef __AIRT_ALGO_PROCESS_H__
#define __AIRT_ALGO_PROCESS_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <airtalgo/format.h>
#include <airtalgo/channel.h>
#include <airtalgo/Algo.h>
#include <chrono>
#include <memory>

namespace airtalgo{
	class Process {
		public:
			Process();
		public:
			/**
			 * @brief Push data in the algo stream.
			 * @param[in] _time Time of the first sample pushed.
			 * @param[in] _data Pointer on the data pushed.
			 * @param[in] _nbChunk Number of chunk present in the pointer.
			 * @return true The procress is done corectly.
			 * @return false An error occured.
			 */
			bool push(std::chrono::system_clock::time_point& _time,
			          void* _data,
			          size_t _nbChunk);
			/**
			 * @brief Push data in the algo stream.
			 * @param[in] _time Time of the first sample requested.
			 * @param[in] _data Pointer on the data pushed.
			 * @param[in,out] _nbChunk Number of chunk present in the pointer (set at the number of chunk requested(hope)).
			 * @return true The procress is done corectly.
			 * @return false An error occured.
			 */
			bool pull(std::chrono::system_clock::time_point& _time,
			          void*& _data,
			          size_t& _nbChunk);
			/**
			 * @brief Push data in the algo stream.
			 * @param[in] _time Time of the first sample pushed.
			 * @param[in] _inData Pointer on the data pushed.
			 * @param[in] _inNbChunk Number of chunk present in the input pointer.
			 * @param[out] _outData Pointer on the data geted.
			 * @param[out] _outNbChunk Number of chunk present in the output pointer.
			 * @return true The procress is done corectly.
			 * @return false An error occured.
			 */
			bool process(std::chrono::system_clock::time_point& _time,
			             void* _inData,
			             size_t _inNbChunk,
			             void*& _outData,
			             size_t& _outNbChunk);
		protected:
			std::vector<std::shared_ptr<airtalgo::Algo> > m_listAlgo;
		public:
			void pushBack(const std::shared_ptr<airtalgo::Algo>& _algo);
			void pushFront(const std::shared_ptr<airtalgo::Algo>& _algo);
			void updateInterAlgo();
			void clear() {
				m_listAlgo.clear();
			}
	};
};

#endif
