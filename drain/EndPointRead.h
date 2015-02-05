/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_END_POINT_READ_H__
#define __AIRT_ALGO_END_POINT_READ_H__

#include <drain/EndPoint.h>

namespace drain{
	class EndPointRead : public EndPoint {
		protected:
			/**
			 * @brief Constructor
			 */
			EndPointRead();
			void init();
		public:
			static std::shared_ptr<EndPointRead> create();
			/**
			 * @brief Destructor
			 */
			virtual ~EndPointRead() {};
			virtual void configurationChange();
			virtual bool process(std::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
	};
};

#endif
