/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <audio/drain/EndPoint.hpp>

namespace audio {
	namespace drain{
		class EndPointRead : public EndPoint {
			protected:
				/**
				 * @brief Constructor
				 */
				EndPointRead();
				void init();
			public:
				static ememory::SharedPtr<EndPointRead> create();
				/**
				 * @brief Destructor
				 */
				virtual ~EndPointRead() {};
				virtual void configurationChange();
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
				/**
				 * @brief Set buffer size in chunk number
				 * @param[in] _nbChunk Number of chunk in the buffer
				 */
				virtual void setBufferSize(size_t _nbChunk);
				/**
				 * @brief Set buffer size size of the buffer with the stored time in µs
				 * @param[in] _time Time in microsecond of the buffer
				 */
				virtual void setBufferSize(const std::chrono::microseconds& _time);
				/**
				 * @brief get buffer size in chunk number
				 * @return Number of chunk that can be written in the buffer
				 */
				virtual size_t getBufferSize();
				/**
				 * @brief Set buffer size size of the buffer with the stored time in µs
				 * @return Time in microsecond that can be written in the buffer
				 */
				virtual std::chrono::microseconds getBufferSizeMicrosecond();
				/**
				 * @brief Get buffer size filled in chunk number
				 * @return Number of chunk in the buffer (that might be read/write)
				 */
				virtual size_t getBufferFillSize();
				/**
				 * @brief Set buffer size size of the buffer with the stored time in µs
				 * @return Time in microsecond of the buffer (that might be read/write)
				 */
				virtual std::chrono::microseconds getBufferFillSizeMicrosecond();
		};
	}
}

