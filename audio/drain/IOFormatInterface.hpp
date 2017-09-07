/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/String.hpp>
#include <etk/Vector.hpp>
#include <cstdint>
#include <audio/format.hpp>
#include <audio/channel.hpp>
#include <chrono>
#include <functional>
#include <ememory/memory.hpp>
#include "AutoLogInOut.hpp"
#include "debug.hpp"

namespace audio {
	namespace drain{
		class IOFormatInterface {
			public:
				IOFormatInterface();
				IOFormatInterface(etk::Vector<enum audio::channel> _map, enum audio::format _format=audio::format_int16, float _frequency=48000.0f);
				void set(etk::Vector<enum audio::channel> _map, enum audio::format _format=audio::format_int16, float _frequency=48000.0f);
			protected:
				bool m_configured;
			public:
				void setConfigured(bool _value);
				bool getConfigured() const;
			protected:
				enum audio::format m_format; //!< input Algo Format
			public:
				/**
				 * @brief Get the algo format.
				 * @return the current Format.
				 */
				enum audio::format getFormat() const;
				/**
				 * @brief Set the algo format.
				 * @param[in] _value New Format.
				 */
				void setFormat(enum audio::format _value);
			protected:
				etk::Vector<enum audio::channel> m_map; //!< input channel Map
			public:
				/**
				 * @brief Get the algo channel Map.
				 * @return the current channel Map.
				 */
				const etk::Vector<enum audio::channel>& getMap() const;
				/**
				 * @brief Set the algo channel Map.
				 * @param[in] _value New channel Map.
				 */
				void setMap(const etk::Vector<enum audio::channel>& _value);
			protected:
				float m_frequency; //!< input Algo Format
			public:
				/**
				 * @brief Get the algo frequency.
				 * @return the current frequency.
				 */
				float getFrequency() const;
				/**
				 * @brief Set the algo frequency.
				 * @param[in] _value New frequency.
				 */
				void setFrequency(float _value);
			public:
				/**
				 * @brief Get the Chunk size in byte.
				 * @return the number of byte used by chunk.
				 */
				int32_t getChunkSize() const;
			protected:
				etk::Function<void()> m_ioChangeFunctor; //!< function pointer on the upper class
				void configurationChange();
			public:
				/**
				 * @brief Set the callback function to be notify when the arameter change.
				 * @param[in] _functor Function to call.
				 */
				void setCallback(const etk::Function<void()>& _functor);
		};
		etk::Stream& operator <<(etk::Stream& _os, const audio::drain::IOFormatInterface& _obj);
	}
}

