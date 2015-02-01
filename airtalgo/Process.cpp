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

namespace std {
static std::ostream& operator <<(std::ostream& _os, const std::vector<float>& _obj) {
	_os << std::string("{");
	for (size_t iii=0; iii<_obj.size(); ++iii) {
		if (iii!=0) {
			_os << std::string(";");
		}
		_os << _obj[iii];
	}
	_os << std::string("}");
	return _os;
}
}

template<typename T> std::vector<T> getUnion(const std::vector<T>& _out, const std::vector<T>& _in) {
	std::vector<T> out;
	if (_out.size() == 0) {
		// Last is ok for all format
		// ==> set the limit with the next element
		out = _in;
	} else if (_in.size() == 0) {
		// next is ok for all format
	} else {
		// must check all values
		for (auto &itOut : _out) {
			for (auto &itIn : _in) {
				if (itOut == itIn) {
					out.push_back(itOut);
				}
			}
		}
	}
	return out;
}

void airtalgo::Process::updateInterAlgo() {
	AIRTALGO_INFO(" display properties : nbAlgo : " << m_listAlgo.size());
	for (auto &it : m_listAlgo) {
		AIRTALGO_INFO("    [" << it->getType() << "] '" << it->getName() << "'");
		if (it->getInputFormat().getConfigured() == true) {
			AIRTALGO_INFO("        Input : " << it->getInputFormat());
		} else {
			AIRTALGO_INFO("        Input : Not configured");
			AIRTALGO_INFO("            format : " << it->getFormatSupportedInput());
			AIRTALGO_INFO("            frequency : " << it->getFrequencySupportedInput());
			AIRTALGO_INFO("            map : " << it->getMapSupportedInput());
		}
		if (it->getOutputFormat().getConfigured() == true) {
			AIRTALGO_INFO("        Output: " << it->getOutputFormat());
		} else {
			AIRTALGO_INFO("        Output : Not configured");
			AIRTALGO_INFO("            format : " << it->getFormatSupportedOutput());
			AIRTALGO_INFO("            frequency : " << it->getFrequencySupportedOutput());
			AIRTALGO_INFO("            map : " << it->getMapSupportedOutput());
		}
	}
	AIRTALGO_INFO("********* configuration START *************");
	for (size_t iii=1; iii<m_listAlgo.size(); ++iii) {
		if (    m_listAlgo[iii-1]->getOutputFormat().getConfigured() == false
		     && m_listAlgo[iii]->getInputFormat().getConfigured() == false) {
			std::vector<float> freq;
			std::vector<std::vector<airtalgo::channel>> map;
			std::vector<airtalgo::format> format;
			// step 1 : check frequency:
			freq = getUnion<float>(m_listAlgo[iii-1]->getFrequencySupportedOutput(),
			                       m_listAlgo[iii]->getFrequencySupportedInput());
			// step 2 : Check map:
			map = getUnion<std::vector<airtalgo::channel>>(m_listAlgo[iii-1]->getMapSupportedOutput(),
			                                               m_listAlgo[iii]->getMapSupportedInput());
			// step 3 : Check Format:
			format = getUnion<airtalgo::format>(m_listAlgo[iii-1]->getFormatSupportedOutput(),
			                                    m_listAlgo[iii]->getFormatSupportedInput());
			
			if (    freq.size() == 1
			     && map.size() == 1
			     && format.size() == 1) {
				AIRTALGO_INFO("        find 1 compatibility :{format=" << format << ",frequency=" << freq << ",map=" << map << "}");
				airtalgo::IOFormatInterface tmp(map[0], format[0], freq[0]);
				m_listAlgo[iii-1]->setOutputFormat(tmp);
				m_listAlgo[iii]->setInputFormat(tmp);
				continue;
			}
			
			AIRTALGO_INFO("        union:");
			AIRTALGO_INFO("            format : " << format);
			AIRTALGO_INFO("            frequency : " << freq);
			AIRTALGO_INFO("            map : " << map);
		} else if (    m_listAlgo[iii-1]->getOutputFormat().getConfigured() == false
		            || m_listAlgo[iii]->getInputFormat().getConfigured() == false) {
			AIRTALGO_ERROR(" configuration error mode in " << iii-1 << " && " << iii );
		}
	}
	AIRTALGO_INFO("********* configuration will be done *************");
	for (auto &it : m_listAlgo) {
		AIRTALGO_INFO("    [" << it->getType() << "] '" << it->getName() << "'");
		if (it->getInputFormat().getConfigured() == true) {
			AIRTALGO_INFO("        Input : " << it->getInputFormat());
		} else {
			AIRTALGO_ERROR("        Input : Not configured");
		}
		if (it->getOutputFormat().getConfigured() == true) {
			AIRTALGO_INFO("        Output: " << it->getOutputFormat());
		} else {
			AIRTALGO_ERROR("        Output : Not configured");
		}
	}
	
	//exit(-1);
}

void airtalgo::Process::removeAlgoDynamic() {
	
}
