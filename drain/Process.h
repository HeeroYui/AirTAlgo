/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#ifndef __AIRT_ALGO_PROCESS_H__
#define __AIRT_ALGO_PROCESS_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <audio/format.h>
#include <audio/channel.h>
#include <drain/Algo.h>
#if __cplusplus >= 201103L
	#include <memory>
	#include <chrono>
#else
	#include <etk/chrono.h>
	#include <etk/memory.h>
#endif

namespace drain{
	class Process {
		protected:
			std::vector<int8_t> m_data; //!< temporary overlap output buffer (change size of the output data)
		public:
			Process();
			virtual ~Process();
		public:
			/**
			 * @brief Push data in the algo stream.
			 * @param[in] _time Time of the first sample pushed.
			 * @param[in] _data Pointer on the data pushed.
			 * @param[in] _nbChunk Number of chunk present in the pointer.
			 * @return true The procress is done corectly.
			 * @return false An error occured.
			 */
			bool push(std11::chrono::system_clock::time_point& _time,
			          void* _data,
			          size_t _nbChunk);
			/**
			 * @brief Push data in the algo stream.
			 * @param[in] _time Time of the first sample requested.
			 * @param[in] _data Pointer on the data pushed.
			 * @param[in,out] _nbChunk Number of chunk present in the pointer (set at the number of chunk requested(hope)).
			 * @param[out] _chunkSize size of a single chunk. TODO : Not needed ... Remove it ...
			 * @return true The procress is done corectly.
			 * @return false An error occured.
			 */
			bool pull(std11::chrono::system_clock::time_point& _time,
			          void* _data,
			          size_t _nbChunk,
			          size_t _chunkSize);
			/**
			 * @brief Push data in the algo stream.
			 * @param[in] _time Time of the first sample pushed.
			 * @param[in] _inData Pointer on the data pushed.
			 * @param[in] _inNbChunk Number of chunk present in the input pointer.
			 * @param[out] _outData Pointer on the data geted.
			 * @param[out] _outNbChunk Number of chunk present in the output pointer.
			 * @return true The procress is done corectly.
			 * @return false An error occured.
			 */
			bool process(std11::chrono::system_clock::time_point& _time,
			             void* _inData,
			             size_t _inNbChunk,
			             void*& _outData,
			             size_t& _outNbChunk);
			bool process(void* _inData,
			             size_t _inNbChunk,
			             void*& _outData,
			             size_t& _outNbChunk) {
				std11::chrono::system_clock::time_point time;
				return process(time, _inData, _inNbChunk, _outData, _outNbChunk);
			}
			bool processIn(void* _inData,
			               size_t _inNbChunk,
			               void* _outData,
			               size_t _outNbChunk);
		protected:
			IOFormatInterface m_inputConfig;
		public:
			const IOFormatInterface& getInputConfig() const {
				return m_inputConfig;
			}
			void setInputConfig(const IOFormatInterface& _interface) {
				m_inputConfig = _interface;
			}
		protected:
			IOFormatInterface m_outputConfig;
		public:
			const IOFormatInterface& getOutputConfig() const {
				return m_outputConfig;
			}
			void setOutputConfig(const IOFormatInterface& _interface) {
				m_outputConfig = _interface;
			}
		protected:
			std::vector<std11::shared_ptr<drain::Algo> > m_listAlgo;
		public:
			void pushBack(const std11::shared_ptr<drain::Algo>& _algo);
			void pushFront(const std11::shared_ptr<drain::Algo>& _algo);
			void clear() {
				m_isConfigured = false;
				m_listAlgo.clear();
			}
			size_t size() {
				return m_listAlgo.size();
			}
			std::vector<std11::shared_ptr<drain::Algo> >::iterator begin() {
				return m_listAlgo.begin();
			}
			std::vector<std11::shared_ptr<drain::Algo> >::iterator end() {
				return m_listAlgo.end();
			}
			
			template<typename T> std11::shared_ptr<T> get(const std::string& _name) {
				for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
					if (m_listAlgo[iii] == nullptr) {
						continue;
					}
					if (m_listAlgo[iii]->getName() == _name) {
						return std11::dynamic_pointer_cast<T>(m_listAlgo[iii]);
					}
				}
				return std11::shared_ptr<T>();
			}
			template<typename T> std11::shared_ptr<const T> get(const std::string& _name) const {
				for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
					if (m_listAlgo[iii] == nullptr) {
						continue;
					}
					if (m_listAlgo[iii]->getName() == _name) {
						return std11::dynamic_pointer_cast<T>(m_listAlgo[iii]);
					}
				}
				return std11::shared_ptr<const T>();
			}
			template<typename T> std11::shared_ptr<T> get(int32_t _id) {
				return std11::dynamic_pointer_cast<T>(m_listAlgo[_id]);
			}
			template<typename T> void removeIfFirst() {
				if (m_listAlgo.size() > 0) {
					std11::shared_ptr<T> algoEP = get<T>(0);
					if (algoEP != nullptr) {
						m_listAlgo.erase(m_listAlgo.begin());
					}
				}
			}
			template<typename T> void removeIfLast() {
				if (m_listAlgo.size() > 0) {
					std11::shared_ptr<T> algoEP = get<T>(m_listAlgo.size()-1);
					if (algoEP != nullptr) {
						m_listAlgo.erase(m_listAlgo.begin()+m_listAlgo.size()-1);
					}
				}
			}
			template<typename T> bool hasType() {
				for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
					std11::shared_ptr<T> tmp = std11::dynamic_pointer_cast<T>(m_listAlgo[iii]);
					if (tmp != nullptr) {
						return true;
					}
				}
				return false;
			}
		private:
			bool m_isConfigured;
		public:
			void updateInterAlgo();
			void removeAlgoDynamic();
		private:
			void displayAlgo();
			void updateAlgo(size_t _position);
	};
};

#endif
