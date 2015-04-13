/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AUDIO_DRAIN_ALGO_ALGO_END_POINT_WRITE_H__
#define __AUDIO_DRAIN_ALGO_ALGO_END_POINT_WRITE_H__

#include <audio/drain/EndPoint.h>
#include <etk/functional.h>
#include <etk/mutex.h>
#include <audio/drain/CircularBuffer.h>

namespace audio {
	namespace drain{
		typedef std11::function<void (const audio::Time& _time,
		                              size_t _nbChunk,
		                              enum audio::format _format,
		                              uint32_t _frequency,
		                              const std::vector<audio::channel>& _map)> playbackFunctionWrite;
		class EndPointWrite : public EndPoint {
			private:
				audio::drain::CircularBuffer m_buffer;
				playbackFunctionWrite m_function;
				std11::mutex m_mutex;
			protected:
				/**
				 * @brief Constructor
				 */
				EndPointWrite();
				void init();
			public:
				static std11::shared_ptr<audio::drain::EndPointWrite> create();
				/**
				 * @brief Destructor
				 */
				virtual ~EndPointWrite() {};
				virtual void configurationChange();
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
				virtual void write(const void* _value, size_t _nbChunk);
				virtual void setCallback(playbackFunctionWrite _function) {
					m_function = _function;
				}
			protected:
				std11::chrono::microseconds m_bufferSizeMicroseconds; // 0 if m_bufferSizeChunk != 0
				size_t m_bufferSizeChunk; // 0 if m_bufferSizeMicroseconds != 0
			public:
				/**
				 * @brief Set buffer size in chunk number
				 * @param[in] _nbChunk Number of chunk in the buffer
				 */
				virtual void setBufferSize(size_t _nbChunk);
				/**
				 * @brief Set buffer size size of the buffer with the stored time in µs
				 * @param[in] _time Time in microsecond of the buffer
				 */
				virtual void setBufferSize(const std11::chrono::microseconds& _time);
				/**
				 * @brief get buffer size in chunk number
				 * @return Number of chunk that can be written in the buffer
				 */
				virtual size_t getBufferSize();
				/**
				 * @brief Set buffer size size of the buffer with the stored time in µs
				 * @return Time in microsecond that can be written in the buffer
				 */
				virtual std11::chrono::microseconds getBufferSizeMicrosecond();
				/**
				 * @brief Get buffer size filled in chunk number
				 * @return Number of chunk in the buffer (that might be read/write)
				 */
				virtual size_t getBufferFillSize();
				/**
				 * @brief Set buffer size size of the buffer with the stored time in µs
				 * @return Time in microsecond of the buffer (that might be read/write)
				 */
				virtual std11::chrono::microseconds getBufferFillSizeMicrosecond();
		};
	}
}

#endif
