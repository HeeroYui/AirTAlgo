/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <airtalgo/format.h>
#include <airtalgo/channel.h>
#include <airtalgo/Process.h>
#include <chrono>

#undef __class__
#define __class__ "Process"

airtalgo::Process::Process() {
	
}
airtalgo::Process::~Process() {
	for (auto &it : m_listAlgo) {
		it.reset();
	}
}

bool airtalgo::Process::push(std::chrono::system_clock::time_point& _time,
                             void* _data,
                             size_t _nbChunk) {
	void* out = nullptr;
	size_t nbChunkOut;
	AIRTALGO_VERBOSE("        Process push");
	process(_time, _data, _nbChunk, out, nbChunkOut);
	return true;
}

bool airtalgo::Process::pull(std::chrono::system_clock::time_point& _time,
                             void* _data,
                             size_t _nbChunk,
                             size_t _chunkSize) {
	#if 0
	void* in = nullptr;
	size_t nbChunkIn = _nbChunk;
	void* out = nullptr;
	size_t nbChunkOut;
	if (nbChunkIn < 128) {
		nbChunkIn = 128;
	}
	for (int32_t iii=m_listAlgo.size()-1; iii >=0; --iii) {
		if (m_listAlgo[iii] != nullptr) {
			nbChunkIn = m_listAlgo[iii]->needInputData(nbChunkIn);
		}
	}
	if (nbChunkIn < 32) {
		nbChunkIn = 32;
	}
	AIRTALGO_VERBOSE("process pull : request out=" << _nbChunk << " input slot request=" << nbChunkIn);
	process(_time, in, nbChunkIn, _data, _nbChunk);
	AIRTALGO_VERBOSE("process pull : real get " << _nbChunk);
	#else
	//std::cout << "        Interface DIRECT " << std::endl;
	while(m_data.size()<_nbChunk*_chunkSize) {
		void* in = NULL;
		size_t nbChunkIn = _nbChunk - m_data.size()/_chunkSize;
		void* out = NULL;
		size_t nbChunkOut;
		if (nbChunkIn < 128) {
			nbChunkIn = 128;
		}
		// TODO : maybe remove this for input data ...
		for (int32_t iii=m_listAlgo.size()-1; iii >=0; --iii) {
			if (m_listAlgo[iii] != NULL) {
				nbChunkIn = m_listAlgo[iii]->needInputData(nbChunkIn);
			}
		}
		if (nbChunkIn < 32) {
			nbChunkIn = 32;
		}
		
		// get data from the upstream
		//std::cout << "         * request " << nbChunkIn << " chunk" << std::endl;
		process(_time, in, nbChunkIn, out, nbChunkOut);
		//std::cout << "         * get " << nbChunkOut << " chunk" << std::endl;
		if (nbChunkOut > 0) {
			size_t position = m_data.size();
			m_data.resize(m_data.size() + nbChunkOut*_chunkSize);
			memcpy(&m_data[position], out, nbChunkOut*_chunkSize);
		} else {
			// TODO : ERROR ...
			break;
		}
	}
	if (m_data.size()>=_nbChunk*_chunkSize) {
		//std::cout << "         * copy needed data" << std::endl;
		memcpy(_data, &m_data[0], _nbChunk*_chunkSize);
		m_data.erase(m_data.begin(), m_data.begin()+_nbChunk*_chunkSize);
	} else {
		//std::cout << "         * soft underflow" << std::endl;
		// ERROR
		m_data.clear();
	}
	#endif
	return true;
}


bool airtalgo::Process::process(std::chrono::system_clock::time_point& _time,
                                void* _inData,
                                size_t _inNbChunk,
                                void*& _outData,
                                size_t& _outNbChunk) {
	if (m_listAlgo.size() == 0) {
		_outData = _inData;
		_outNbChunk = _inNbChunk;
		return true;
	}
	AIRTALGO_VERBOSE(" process : " << m_listAlgo.size() << " algos nbChunk=" << _outNbChunk);
	for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
		//std::cout << "            Algo " << iii+1 << "/" << m_listAlgo.size() << std::endl;
		if (m_listAlgo[iii] != nullptr) {
			m_listAlgo[iii]->process(_time, _inData, _inNbChunk, _outData, _outNbChunk);
			_inData = _outData;
			_inNbChunk = _outNbChunk;
		}
	}
	return true;
}

void airtalgo::Process::pushBack(const std::shared_ptr<airtalgo::Algo>& _algo) {
	m_listAlgo.push_back(_algo);
}

void airtalgo::Process::pushFront(const std::shared_ptr<airtalgo::Algo>& _algo) {
	m_listAlgo.insert(m_listAlgo.begin(), _algo);
}

void airtalgo::Process::updateInterAlgo() {
	// TODO : ...
}
