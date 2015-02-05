/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <audio/format.h>
#include <audio/channel.h>
#include <drain/Process.h>
#include <drain/ChannelReorder.h>
#include <drain/FormatUpdate.h>
#include <drain/Resampler.h>
#include <chrono>

#undef __class__
#define __class__ "Process"

drain::Process::Process() :
  m_isConfigured(false) {
	
}
drain::Process::~Process() {
	for (auto &it : m_listAlgo) {
		it.reset();
	}
}

bool drain::Process::push(std::chrono::system_clock::time_point& _time,
                             void* _data,
                             size_t _nbChunk) {
	void* out = nullptr;
	size_t nbChunkOut;
	DRAIN_VERBOSE("        Process push");
	process(_time, _data, _nbChunk, out, nbChunkOut);
	return true;
}

bool drain::Process::pull(std::chrono::system_clock::time_point& _time,
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


bool drain::Process::process(std::chrono::system_clock::time_point& _time,
                                void* _inData,
                                size_t _inNbChunk,
                                void*& _outData,
                                size_t& _outNbChunk) {
	if (m_listAlgo.size() == 0) {
		_outData = _inData;
		_outNbChunk = _inNbChunk;
		return true;
	}
	DRAIN_VERBOSE(" process : " << m_listAlgo.size() << " algos nbChunk=" << _outNbChunk);
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

void drain::Process::pushBack(const std::shared_ptr<drain::Algo>& _algo) {
	removeAlgoDynamic();
	m_listAlgo.push_back(_algo);
}

void drain::Process::pushFront(const std::shared_ptr<drain::Algo>& _algo) {
	removeAlgoDynamic();
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

void drain::Process::updateInterAlgo() {
	if (m_isConfigured == true) {
		// cahin is already configured
		return ;
	}
	DRAIN_INFO(" display properties : nbAlgo : " << m_listAlgo.size());
	for (auto &it : m_listAlgo) {
		DRAIN_INFO("    [" << it->getType() << "] '" << it->getName() << "'");
		if (it->getInputFormat().getConfigured() == true) {
			DRAIN_INFO("        Input : " << it->getInputFormat());
		} else {
			DRAIN_INFO("        Input : Not configured");
			DRAIN_INFO("            format : " << it->getFormatSupportedInput());
			DRAIN_INFO("            frequency : " << it->getFrequencySupportedInput());
			DRAIN_INFO("            map : " << it->getMapSupportedInput());
		}
		if (it->getOutputFormat().getConfigured() == true) {
			DRAIN_INFO("        Output: " << it->getOutputFormat());
		} else {
			DRAIN_INFO("        Output : Not configured");
			DRAIN_INFO("            format : " << it->getFormatSupportedOutput());
			DRAIN_INFO("            frequency : " << it->getFrequencySupportedOutput());
			DRAIN_INFO("            map : " << it->getMapSupportedOutput());
		}
	}
	DRAIN_INFO("********* configuration START *************");
	for (size_t iii=1; iii<m_listAlgo.size(); ++iii) {
		if (    m_listAlgo[iii-1]->getOutputFormat().getConfigured() == false
		     && m_listAlgo[iii]->getInputFormat().getConfigured() == false) {
			// step 1 : check frequency:
			std::vector<float> freqOut = m_listAlgo[iii-1]->getFrequencySupportedOutput();
			std::vector<float> freqIn = m_listAlgo[iii]->getFrequencySupportedInput();
			std::vector<float> freq = getUnion<float>(freqOut, freqIn);
			// step 2 : Check map:
			std::vector<std::vector<audio::channel>> mapOut = m_listAlgo[iii-1]->getMapSupportedOutput();
			std::vector<std::vector<audio::channel>> mapIn = m_listAlgo[iii]->getMapSupportedInput();
			std::vector<std::vector<audio::channel>> map = getUnion<std::vector<audio::channel>>(mapOut, mapIn);
			// step 3 : Check Format:
			std::vector<audio::format> formatOut = m_listAlgo[iii-1]->getFormatSupportedOutput();
			std::vector<audio::format> formatIn = m_listAlgo[iii]->getFormatSupportedInput();
			std::vector<audio::format> format = getUnion<audio::format>(formatOut, formatIn);
			
			if (    freq.size() >= 1
			     && map.size() >= 1
			     && format.size() >= 1) {
				DRAIN_INFO("        find 1 compatibility :{format=" << format << ",frequency=" << freq << ",map=" << map << "}");
				drain::IOFormatInterface tmp(map[0], format[0], freq[0]);
				m_listAlgo[iii-1]->setOutputFormat(tmp);
				m_listAlgo[iii]->setInputFormat(tmp);
				continue;
			}
			// create mapping to transform:
			drain::IOFormatInterface out;
			drain::IOFormatInterface in;
			if (freq.size() > 0) {
				out.setFrequency(freq[0]);
				in.setFrequency(freq[0]);
			} else {
				if (freqOut.size() == 0) {
					if (freqIn.size() == 0) {
						out.setFrequency(m_listAlgo[iii-1]->getInputFormat().getFrequency());
						in.setFrequency(m_listAlgo[iii-1]->getInputFormat().getFrequency());
					} else {
						out.setFrequency(freqIn[0]);
						in.setFrequency(freqIn[0]);
					}
				} else {
					if (freqIn.size() == 0) {
						out.setFrequency(freqOut[0]);
						in.setFrequency(freqOut[0]);
					} else {
						out.setFrequency(freqOut[0]);
						in.setFrequency(freqIn[0]);
					}
				}
			}
			if (map.size() > 0) {
				out.setMap(map[0]);
				in.setMap(map[0]);
			} else {
				if (mapOut.size() == 0) {
					if (mapIn.size() == 0) {
						out.setMap(m_listAlgo[iii-1]->getInputFormat().getMap());
						in.setMap(m_listAlgo[iii-1]->getInputFormat().getMap());
					} else {
						out.setMap(mapIn[0]);
						in.setMap(mapIn[0]);
					}
				} else {
					if (mapIn.size() == 0) {
						out.setMap(mapOut[0]);
						in.setMap(mapOut[0]);
					} else {
						out.setMap(mapOut[0]);
						in.setMap(mapIn[0]);
					}
				}
			}
			if (format.size() > 0) {
				out.setFormat(format[0]);
				in.setFormat(format[0]);
			} else {
				if (formatOut.size() == 0) {
					if (formatIn.size() == 0) {
						out.setFormat(m_listAlgo[iii-1]->getInputFormat().getFormat());
						in.setFormat(m_listAlgo[iii-1]->getInputFormat().getFormat());
					} else {
						out.setFormat(formatIn[0]);
						in.setFormat(formatIn[0]);
					}
				} else {
					if (formatIn.size() == 0) {
						out.setFormat(formatOut[0]);
						in.setFormat(formatOut[0]);
					} else {
						out.setFormat(formatOut[0]);
						in.setFormat(formatIn[0]);
					}
				}
			}
			DRAIN_INFO("        union:");
			DRAIN_INFO("            format : " << format);
			DRAIN_INFO("            frequency : " << freq);
			DRAIN_INFO("            map : " << map);
			DRAIN_INFO("        update: out=" << out);
			DRAIN_INFO("                in=" << in);
			m_listAlgo[iii-1]->setOutputFormat(out);
			m_listAlgo[iii]->setInputFormat(in);
			// TODO : Add updater with an optimisation of CPU
			if (out.getFrequency() != in.getFrequency()) {
				
				// TODO : Do it better: special check for resampler : only support int16_t
				if (    out.getFormat() != audio::format_int16
				     /* && out.getFormat() != format_float */) {
					// need add a format Updater
					std::shared_ptr<drain::FormatUpdate> algo = drain::FormatUpdate::create();
					algo->setTemporary();
					algo->setInputFormat(out);
					out.setFormat(audio::format_int16);
					algo->setOutputFormat(out);
					m_listAlgo.insert(m_listAlgo.begin()+iii, algo);
					DRAIN_INFO("convert " << out.getFormat() << " -> " << in.getFormat());
					iii++;
				}
				// need add a resampler
				std::shared_ptr<drain::Resampler> algo = drain::Resampler::create();
				algo->setTemporary();
				algo->setInputFormat(out);
				out.setFrequency(in.getFrequency());
				algo->setOutputFormat(out);
				m_listAlgo.insert(m_listAlgo.begin()+iii, algo);
				DRAIN_INFO("convert " << out.getFrequency() << " -> " << in.getFrequency());
				out.setFrequency(in.getFrequency());
				iii++;
			}
			if (out.getMap() != in.getMap()) {
				// need add a channel Reorder
				std::shared_ptr<drain::ChannelReorder> algo = drain::ChannelReorder::create();
				algo->setTemporary();
				algo->setInputFormat(out);
				out.setMap(in.getMap());
				algo->setOutputFormat(out);
				m_listAlgo.insert(m_listAlgo.begin()+iii, algo);
				DRAIN_INFO("convert " << out.getMap() << " -> " << in.getMap());
				iii++;
			}
			if (out.getFormat() != in.getFormat()) {
				// need add a format Updater
				std::shared_ptr<drain::FormatUpdate> algo = drain::FormatUpdate::create();
				algo->setTemporary();
				algo->setInputFormat(out);
				out.setFormat(in.getFormat());
				algo->setOutputFormat(out);
				m_listAlgo.insert(m_listAlgo.begin()+iii, algo);
				DRAIN_INFO("convert " << out.getFormat() << " -> " << in.getFormat());
				iii++;
			}
			
		} else if (    m_listAlgo[iii-1]->getOutputFormat().getConfigured() == false
		            || m_listAlgo[iii]->getInputFormat().getConfigured() == false) {
			DRAIN_ERROR(" configuration error mode in " << iii-1 << " && " << iii );
		}
	}
	DRAIN_INFO("********* configuration will be done *************");
	for (auto &it : m_listAlgo) {
		DRAIN_INFO("    [" << it->getType() << "] '" << it->getName() << "'");
		if (it->getInputFormat().getConfigured() == true) {
			DRAIN_INFO("        Input : " << it->getInputFormat());
		} else {
			DRAIN_ERROR("        Input : Not configured");
		}
		if (it->getOutputFormat().getConfigured() == true) {
			DRAIN_INFO("        Output: " << it->getOutputFormat());
		} else {
			DRAIN_ERROR("        Output : Not configured");
		}
	}
	m_isConfigured = true;
	//exit(-1);
}

void drain::Process::removeAlgoDynamic() {
	if (m_isConfigured == true) {
		// chain is already unconfigured.
		return;
	}
	m_isConfigured = false;
}
