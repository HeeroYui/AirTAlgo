/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <audio/drain/Algo.hpp>

namespace audio {
	namespace drain {
		class FormatUpdate : public Algo {
			protected:
				/**
				 * @brief Constructor
				 */
				FormatUpdate();
				void init();
			public:
				static ememory::SharedPtr<audio::drain::FormatUpdate> create();
				/**
				 * @brief Destructor
				 */
				virtual ~FormatUpdate() {};
			protected:
				virtual void configurationChange();
			public:
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
			private:
				void (*m_functionConvert)(void* _input, void* _output, size_t _nbSample);
		};
	}
}

