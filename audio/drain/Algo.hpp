/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <audio/format.hpp>
#include <audio/channel.hpp>
#include <chrono>
#include <functional>
#include <ememory/memory.hpp>
#include "AutoLogInOut.hpp"
#include "IOFormatInterface.hpp"
#include <audio/Time.hpp>
#include <audio/Duration.hpp>
#include "debug.hpp"

/**
 * @brief Audio library namespace
 */
namespace audio {
	/**
	 * @brief audio-algo library namespace
	 */
	namespace drain{
		typedef std::function<void (const std::string& _origin, const std::string& _status)> algoStatusFunction;
		class Algo : public ememory::EnableSharedFromThis<Algo> {
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
				void setType(const std::string& _type) {
					m_type = _type;
				}
			public:
				virtual std::string getDotDesc();
			private:
				bool m_temporary;
			public:
				void setTemporary() {
					m_temporary = true;
				}
				bool getTemporary() const {
					return m_temporary;
				}
			private:
				algoStatusFunction m_statusFunction;
			public:
				void setStatusFunction(algoStatusFunction _newFunction);
			protected:
				void generateStatus(const std::string& _status);
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
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk) = 0;
				/**
				 * @brief To approximatively have the correct input of data of every algo ge call it to have an aproxomation of input data needed.
				 * @param[in] _output number of chunk estimate in output
				 * @return number of sample needed to have nearly the good number of sample
				 */
				// TODO : Manage the change of the timestamp ...
				virtual size_t needInputData(size_t _output);
			protected: // note when nothing ==> support all type
				std::vector<audio::format> m_supportedFormat;
			public:
				virtual std::vector<audio::format> getFormatSupportedInput() {
					if (m_output.getConfigured() == true) {
						std::vector<audio::format> out;
						out.push_back(m_output.getFormat());
						return out;
					}
					return m_supportedFormat;
				};
				virtual std::vector<audio::format> getFormatSupportedOutput() {
					if (m_input.getConfigured() == true) {
						std::vector<audio::format> out;
						out.push_back(m_input.getFormat());
						return out;
					}
					return m_supportedFormat;
				};
			protected: // note when nothing ==> support all type
				std::vector<std::vector<audio::channel> > m_supportedMap;
			public:
				virtual std::vector<std::vector<audio::channel> > getMapSupportedInput() {
					if (m_output.getConfigured() == true) {
						std::vector<std::vector<audio::channel> > out;
						out.push_back(m_output.getMap());
						return out;
					}
					return m_supportedMap;
				};
				virtual std::vector<std::vector<audio::channel> > getMapSupportedOutput() {
					if (m_input.getConfigured() == true) {
						std::vector<std::vector<audio::channel> > out;
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
			public:
				/**
				 * @brief Set a parameter in the stream flow
				 * @param[in] _parameter Parameter name.
				 * @param[in] _value Value to set.
				 * @return true set done
				 * @return false An error occured
				 */
				virtual bool setParameter(const std::string& _parameter, const std::string& _value) { return false; }
				/**
				 * @brief Get a parameter value
				 * @param[in] _parameter Parameter name.
				 * @return The requested value.
				 */
				virtual std::string getParameter(const std::string& _parameter) const { return "[ERROR]"; }
				/**
				 * @brief Get a parameter value
				 * @param[in] _parameter Parameter name.
				 * @return The requested value.
				 */
				virtual std::string getParameterProperty(const std::string& _parameter) const { return "[ERROR]"; };
		};
	}
}
#include "debugRemove.hpp"

