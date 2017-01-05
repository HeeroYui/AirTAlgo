/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <audio/drain/EndPoint.hpp>
#include <functional>

namespace audio {
	namespace drain {
		typedef std::function<void (void* _data,
		                              const audio::Time& _playTime,
		                              size_t _nbChunk,
		                              enum audio::format _format,
		                              uint32_t _frequency,
		                              const std::vector<audio::channel>& _map)> playbackFunction;
		typedef std::function<void (const void* _data,
		                              const audio::Time& _readTime,
		                              size_t _nbChunk,
		                              enum audio::format _format,
		                              uint32_t _frequency,
		                              const std::vector<audio::channel>& _map)> recordFunction;
		class EndPointCallback : public EndPoint {
			private:
				playbackFunction m_outputFunction;
				recordFunction m_inputFunction;
			protected:
				/**
				 * @brief Constructor
				 */
				EndPointCallback();
				void init(playbackFunction _callback);
				void init(recordFunction _callback);
			public:
				static ememory::SharedPtr<EndPointCallback> create(playbackFunction _callback);
				static ememory::SharedPtr<EndPointCallback> create(recordFunction _callback);
				/**
				 * @brief Destructor
				 */
				virtual ~EndPointCallback() {};
				virtual void configurationChange();
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
		};
	}
}

