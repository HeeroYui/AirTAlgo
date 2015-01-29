/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_CHANNEL_REORDER_H__
#define __AIRT_ALGO_CHANNEL_REORDER_H__

#include <airtalgo/Algo.h>

namespace airtalgo{
	class ChannelReorder : public Algo {
		protected:
			/**
			 * @brief Constructor
			 */
			ChannelReorder();
			void init();
		public:
			static std::shared_ptr<ChannelReorder> create();
			/**
			 * @brief Destructor
			 */
			virtual ~ChannelReorder() {};
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
