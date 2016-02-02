/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <audio/drain/Algo.h>

namespace audio {
	namespace drain{
		class ChannelReorder : public audio::drain::Algo {
			protected:
				/**
				 * @brief Constructor
				 */
				ChannelReorder();
				void init();
			public:
				static std11::shared_ptr<audio::drain::ChannelReorder> create();
				/**
				 * @brief Destructor
				 */
				virtual ~ChannelReorder() {};
			protected:
				virtual void configurationChange();
			public:
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
		};
	}
}

