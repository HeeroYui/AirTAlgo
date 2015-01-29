/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#ifndef __AIRT_ALGO_CORE_ALGO_H__
#define __AIRT_ALGO_CORE_ALGO_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <airtalgo/format.h>
#include <airtalgo/channel.h>
#include <chrono>
#include <functional>
#include <memory>
#include "debug.h"

namespace airtalgo{
	class autoLogInOut {
		private:
			std::string m_value;
		public:
			autoLogInOut(const std::string& _value) :
			  m_value(_value) {
				AIRTALGO_VERBOSE("                 '" << m_value << "' [START]");
			}
			~autoLogInOut() {
				AIRTALGO_VERBOSE("                 '" << m_value << "' [STOP]");
			}
	};
	class IOFormatInterface {
		public:
			IOFormatInterface() :
			  m_format(airtalgo::format_int16),
			  m_map(),
			  m_frequency(48000) {
				m_map.push_back(airtalgo::channel_frontLeft);
				m_map.push_back(airtalgo::channel_frontRight);
			}
			IOFormatInterface(std::vector<airtalgo::channel> _map, airtalgo::format _format=airtalgo::format_int16, float _frequency=48000) :
			  m_format(_format),
			  m_map(_map),
			  m_frequency(_frequency) {
				
			}
		protected:
			airtalgo::format m_format; //!< input Algo Format
		public:
			/**
			 * @brief Get the algo format.
			 * @return the current Format.
			 */
			airtalgo::format getFormat() const {
				return m_format;
			}
			/**
			 * @brief Set the algo format.
			 * @param[in] _value New Format.
			 */
			void setFormat(airtalgo::format _value) {
				if (m_format == _value) {
					return;
				}
				m_format = _value;
				configurationChange();
			}
		protected:
			std::vector<airtalgo::channel> m_map; //!< input channel Map
		public:
			/**
			 * @brief Get the algo channel Map.
			 * @return the current channel Map.
			 */
			const std::vector<airtalgo::channel>& getMap() const{
				return m_map;
			}
			/**
			 * @brief Set the algo channel Map.
			 * @param[in] _value New channel Map.
			 */
			void setMap(const std::vector<airtalgo::channel>& _value) {
				AIRTALGO_DEBUG(" base : " << m_map << " " << _value);
				if (m_map == _value) {
					return;
				}
				m_map = _value;
				AIRTALGO_DEBUG(" base2 : " << m_map);
				configurationChange();
			}
		protected:
			float m_frequency; //!< input Algo Format
		public:
			/**
			 * @brief Get the algo frequency.
			 * @return the current frequency.
			 */
			float getFrequency() const{
				return m_frequency;
			}
			/**
			 * @brief Set the algo frequency.
			 * @param[in] _value New frequency.
			 */
			void setFrequency(float _value) {
				if (m_frequency == _value) {
					return;
				}
				m_frequency = _value;
				configurationChange();
			}
		protected:
			std::function<void()> m_ioChangeFunctor; //!< function pointer on the upper class
			void configurationChange() {
				if (m_ioChangeFunctor != nullptr) {
					m_ioChangeFunctor();
				}
			}
		public:
			/**
			 * @brief Set the callback function to be notify when the arameter change.
			 * @param[in] _functor Function to call.
			 */
			void setCallback(const std::function<void()>& _functor) {
				m_ioChangeFunctor = _functor;
			}
			
	};
	
	class Algo : public std::enable_shared_from_this<Algo> {
		protected:
			std::vector<int8_t> m_outputData;
			int8_t m_formatSize; //!< sample size
			/**
			 * @brief Constructor
			 */
			Algo();
			void init();
		public:
			/**
			 * @brief Destructor
			 */
			virtual ~Algo() {};
		protected:
			bool m_needProcess; //!< if no change, then no need to process, just forward buffer...
			IOFormatInterface m_input; //!< Input audio property
		public:
			void setInputFormat(const IOFormatInterface& _format) {
				m_input.setFormat(_format.getFormat());
				m_input.setFrequency(_format.getFrequency());
				m_input.setMap(_format.getMap());
			}
			const IOFormatInterface& getInputFormat() const {
				return m_input;
			}
		protected:
			IOFormatInterface m_output; //!< Output audio property
		public:
			void setOutputFormat(const IOFormatInterface& _format) {
				m_output.setFormat(_format.getFormat());
				m_output.setFrequency(_format.getFrequency());
				m_output.setMap(_format.getMap());
			}
			const IOFormatInterface& getOutputFormat() const {
				return m_output;
			}
			/**
			 * @brief Called when a parameter change
			 */
			virtual void configurationChange();
		public:
			/**
			 * @brief Process Algorithm
			 * @param[in] _time Current buffer Time
			 * @param[in] _input input pointer data (not free)
			 * @param[in] _inputNbChunk Number of chunk in the buffer
			 * @param[in] _output Output buffer (provide here) Do not free (can be the input buffer)
			 * @param[in] _outputNbChunk Number of chunk in the output buffer
			 * @return true The process is done corectly
			 * @return false An error occured
			 */
			virtual bool process(std::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk) = 0;
			/**
			 * @brief To approximatively have the correct input of data of every algo ge call it to have an aproxomation of input data needed.
			 * @param[in] _output number of chunk estimate in output
			 * @return number of sample needed to have nearly the good number of sample
			 */
			virtual size_t needInputData(size_t _output);
	};
};
#include "debugRemove.h"

#endif
