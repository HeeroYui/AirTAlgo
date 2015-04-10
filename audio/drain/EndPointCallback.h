/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AUDIO_DRAIN_ALGO_END_POINT_CALLBACK_H__
#define __AUDIO_DRAIN_ALGO_END_POINT_CALLBACK_H__

#include <audio/drain/EndPoint.h>
#include <etk/functional.h>

namespace audio {
	namespace drain {
		typedef std11::function<void (void* _data,
		                              const std11::chrono::system_clock::time_point& _playTime,
		                              size_t _nbChunk,
		                              enum audio::format _format,
		                              uint32_t _frequency,
		                              const std::vector<audio::channel>& _map)> playbackFunction;
		typedef std11::function<void (const void* _data,
		                              const std11::chrono::system_clock::time_point& _readTime,
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
				static std11::shared_ptr<EndPointCallback> create(playbackFunction _callback);
				static std11::shared_ptr<EndPointCallback> create(recordFunction _callback);
				/**
				 * @brief Destructor
				 */
				virtual ~EndPointCallback() {};
				virtual void configurationChange();
				virtual bool process(std11::chrono::system_clock::time_point& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
		};
	}
}

#endif
