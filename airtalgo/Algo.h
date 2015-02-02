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
#include "AutoLogInOut.h"
#include "IOFormatInterface.h"
#include "debug.h"

namespace airtalgo{
	class Algo : public std::enable_shared_from_this<Algo> {
		private:
			std::string m_name;
		public:
			const std::string& getName() const {
				return m_name;
			}
			void setName(const std::string& _name) {
				m_name = _name;
			}
		protected:
			std::string m_type;
		public:
			const std::string& getType() const {
				return m_type;
			}
			void setType(const std::string& _name) {
				m_type = _name;
			}
		private:
			bool m_temporary;
		public:
			void setTemporary() {
				m_temporary = true;
			}
			bool getTemporary() const {
				return m_temporary;
			}
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
				m_input.set(_format.getMap(), _format.getFormat(), _format.getFrequency());
			}
			const IOFormatInterface& getInputFormat() const {
				return m_input;
			}
			IOFormatInterface& getInputFormat() {
				return m_input;
			}
		protected:
			IOFormatInterface m_output; //!< Output audio property
		public:
			void setOutputFormat(const IOFormatInterface& _format) {
				m_output.set(_format.getMap(), _format.getFormat(), _format.getFrequency());
			}
			const IOFormatInterface& getOutputFormat() const {
				return m_output;
			}
			IOFormatInterface& getOutputFormat() {
				return m_output;
			}
		private:
			void configurationChangeLocal() {
				if (    m_output.getConfigured() == true
				     && m_output.getConfigured() == true) {
					configurationChange();
				}
			}
		public:
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
		protected: // note when nothing ==> support all type
			std::vector<airtalgo::format> m_supportedFormat;
		public:
			virtual std::vector<airtalgo::format> getFormatSupportedInput() {
				if (m_output.getConfigured() == true) {
					std::vector<airtalgo::format> out;
					out.push_back(m_output.getFormat());
					return out;
				}
				return m_supportedFormat;
			};
			virtual std::vector<airtalgo::format> getFormatSupportedOutput() {
				if (m_input.getConfigured() == true) {
					std::vector<airtalgo::format> out;
					out.push_back(m_input.getFormat());
					return out;
				}
				return m_supportedFormat;
			};
		protected: // note when nothing ==> support all type
			std::vector<std::vector<airtalgo::channel>> m_supportedMap;
		public:
			virtual std::vector<std::vector<airtalgo::channel>> getMapSupportedInput() {
				if (m_output.getConfigured() == true) {
					std::vector<std::vector<airtalgo::channel>> out;
					out.push_back(m_output.getMap());
					return out;
				}
				return m_supportedMap;
			};
			virtual std::vector<std::vector<airtalgo::channel>> getMapSupportedOutput() {
				if (m_input.getConfigured() == true) {
					std::vector<std::vector<airtalgo::channel>> out;
					out.push_back(m_input.getMap());
					return out;
				}
				return m_supportedMap;
			};
		protected: // note when nothing ==> support all type
			std::vector<float> m_supportedFrequency;
		public:
			virtual std::vector<float> getFrequencySupportedInput() {
				if (m_output.getConfigured() == true) {
					std::vector<float> out;
					out.push_back(m_output.getFrequency());
					return out;
				}
				return m_supportedFrequency;
			};
			virtual std::vector<float> getFrequencySupportedOutput() {
				if (m_input.getConfigured() == true) {
					std::vector<float> out;
					out.push_back(m_input.getFrequency());
					return out;
				}
				return m_supportedFrequency;
			};
	};
};
#include "debugRemove.h"

#endif
