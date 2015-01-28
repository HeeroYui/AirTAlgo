/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_END_POINT_READ_H__
#define __AIRT_ALGO_END_POINT_READ_H__

#include <airtalgo/EndPoint.h>

namespace airtalgo{
	class EndPointRead : public EndPoint {
		public:
			/**
			 * @brief Constructor
			 */
			EndPointRead();
			/**
			 * @brief Destructor
			 */
			virtual ~EndPointRead();
			virtual void configurationChange();
			virtual bool process(std::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
	};
};

#endif
