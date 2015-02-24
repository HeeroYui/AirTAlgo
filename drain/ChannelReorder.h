/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_CHANNEL_REORDER_H__
#define __AIRT_ALGO_CHANNEL_REORDER_H__

#include <drain/Algo.h>

namespace drain{
	class ChannelReorder : public Algo {
		protected:
			/**
			 * @brief Constructor
			 */
			ChannelReorder();
			void init();
		public:
			static std11::shared_ptr<ChannelReorder> create();
			/**
			 * @brief Destructor
			 */
			virtual ~ChannelReorder() {};
		protected:
			virtual void configurationChange();
		public:
			virtual bool process(std11::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
	};
};

#endif
