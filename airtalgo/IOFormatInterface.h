/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#ifndef __AIRT_ALGO_IO_FORMAT_INTERFACE_H__
#define __AIRT_ALGO_IO_FORMAT_INTERFACE_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <airtalgo/format.h>
#include <airtalgo/channel.h>
#include <chrono>
#include <functional>
#include <memory>
#include "AutoLogInOut.h"
#include "debug.h"

namespace airtalgo{
	class IOFormatInterface {
		public:
			IOFormatInterface();
			IOFormatInterface(std::vector<airtalgo::channel> _map, airtalgo::format _format=airtalgo::format_int16, float _frequency=48000.0f);
			void set(std::vector<airtalgo::channel> _map, airtalgo::format _format=airtalgo::format_int16, float _frequency=48000.0f);
		protected:
			bool m_configured;
		public:
			void setConfigured(bool _value);
			bool getConfigured() const;
		protected:
			airtalgo::format m_format; //!< input Algo Format
		public:
			/**
			 * @brief Get the algo format.
			 * @return the current Format.
			 */
			airtalgo::format getFormat() const;
			/**
			 * @brief Set the algo format.
			 * @param[in] _value New Format.
			 */
			void setFormat(airtalgo::format _value);
		protected:
			std::vector<airtalgo::channel> m_map; //!< input channel Map
		public:
			/**
			 * @brief Get the algo channel Map.
			 * @return the current channel Map.
			 */
			const std::vector<airtalgo::channel>& getMap() const;
			/**
			 * @brief Set the algo channel Map.
			 * @param[in] _value New channel Map.
			 */
			void setMap(const std::vector<airtalgo::channel>& _value);
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
		protected:
			std::function<void()> m_ioChangeFunctor; //!< function pointer on the upper class
			void configurationChange();
		public:
			/**
			 * @brief Set the callback function to be notify when the arameter change.
			 * @param[in] _functor Function to call.
			 */
			void setCallback(const std::function<void()>& _functor);
	};
	std::ostream& operator <<(std::ostream& _os, const IOFormatInterface& _obj);
}

#endif
