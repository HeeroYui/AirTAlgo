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

airtalgo::Process::Process() {
	
}

bool airtalgo::Process::push(std::chrono::system_clock::time_point& _time,
                             void* _data,
                             size_t _nbChunk) {
	void* out = nullptr;
	size_t nbChunkOut;
	AIRTALGO_VERBOSE("        Interface DIRECT ");
	process(_time, _data, _nbChunk, out, nbChunkOut);
	return true;
}

bool airtalgo::Process::pull(std::chrono::system_clock::time_point& _time,
                             void*& _data,
                             size_t& _nbChunk) {
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
	process(_time, in, nbChunkIn, _data, _nbChunk);
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
