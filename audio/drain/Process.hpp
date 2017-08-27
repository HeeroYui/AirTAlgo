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
#include <audio/drain/Algo.hpp>
#include <chrono>
#include <ememory/memory.hpp>
#include <etk/os/FSNode.hpp>

namespace audio {
	namespace drain{
		typedef std::function<void (const etk::String& _origin, const etk::String& _status)> statusFunction;
		class Process {
			protected:
				etk::Vector<int8_t> m_data; //!< temporary overlap output buffer (change size of the output data)
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
				bool push(audio::Time& _time,
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
				bool pull(audio::Time& _time,
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
				bool process(audio::Time& _time,
				             void* _inData,
				             size_t _inNbChunk,
				             void*& _outData,
				             size_t& _outNbChunk);
				bool process(void* _inData,
				             size_t _inNbChunk,
				             void*& _outData,
				             size_t& _outNbChunk) {
					audio::Time time;
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
				etk::Vector<ememory::SharedPtr<drain::Algo> > m_listAlgo;
			public:
				void pushBack(ememory::SharedPtr<drain::Algo> _algo);
				void pushFront(ememory::SharedPtr<drain::Algo> _algo);
				void clear() {
					m_isConfigured = false;
					m_listAlgo.clear();
				}
				size_t size() {
					return m_listAlgo.size();
				}
				etk::Vector<ememory::SharedPtr<drain::Algo> >::iterator begin() {
					return m_listAlgo.begin();
				}
				etk::Vector<ememory::SharedPtr<drain::Algo> >::iterator end() {
					return m_listAlgo.end();
				}
				ememory::SharedPtr<drain::Algo> operator[](int32_t _id) {
					return m_listAlgo[_id];
				}
				
				template<typename T> ememory::SharedPtr<T> get(const etk::String& _name) {
					for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
						if (m_listAlgo[iii] == nullptr) {
							continue;
						}
						if (m_listAlgo[iii]->getName() == _name) {
							return ememory::dynamicPointerCast<T>(m_listAlgo[iii]);
						}
					}
					return ememory::SharedPtr<T>();
				}
				template<typename T> ememory::SharedPtr<const T> get(const etk::String& _name) const {
					for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
						if (m_listAlgo[iii] == nullptr) {
							continue;
						}
						if (m_listAlgo[iii]->getName() == _name) {
							return ememory::dynamicPointerCast<T>(m_listAlgo[iii]);
						}
					}
					return ememory::SharedPtr<const T>();
				}
				template<typename T> ememory::SharedPtr<T> get(int32_t _id) {
					return ememory::dynamicPointerCast<T>(m_listAlgo[_id]);
				}
				template<typename T> void removeIfFirst() {
					if (m_listAlgo.size() > 0) {
						ememory::SharedPtr<T> algoEP = get<T>(0);
						if (algoEP != nullptr) {
							m_listAlgo.erase(m_listAlgo.begin());
						}
					}
				}
				template<typename T> void removeIfLast() {
					if (m_listAlgo.size() > 0) {
						ememory::SharedPtr<T> algoEP = get<T>(m_listAlgo.size()-1);
						if (algoEP != nullptr) {
							m_listAlgo.erase(m_listAlgo.begin()+m_listAlgo.size()-1);
						}
					}
				}
				template<typename T> bool hasType() {
					for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
						ememory::SharedPtr<T> tmp = ememory::dynamicPointerCast<T>(m_listAlgo[iii]);
						if (tmp != nullptr) {
							return true;
						}
					}
					return false;
				}
			private:
				statusFunction m_statusFunction;
			public:
				void generateStatus(const etk::String& _origin, const etk::String& _status);
				void setStatusFunction(statusFunction _newFunction);
			private:
				bool m_isConfigured;
			public:
				void updateInterAlgo();
				void removeAlgoDynamic();
			private:
				void displayAlgo();
				void updateAlgo(size_t _position);
			public:
				void generateDot(etk::FSNode& _node, int32_t _offset, int32_t _basicID, etk::String& _nameIn, etk::String& _nameOut, bool _reserseGraph);
				// TODO : Remove this one when we find a good way to do it ...
				void generateDotProcess(etk::FSNode& _node, int32_t _offset, int32_t _basicID, etk::String& _nameIn, etk::String& _nameOut, bool _reserseGraph);
		};
	}
}

