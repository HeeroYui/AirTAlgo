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
	m_data.clear();
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
		DRAIN_WARNING("         * soft underflow");
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
	updateInterAlgo();
	if (m_listAlgo.size() == 0) {
		_outData = _inData;
		_outNbChunk = _inNbChunk;
		return true;
	}
	DRAIN_VERBOSE(" process : " << m_listAlgo.size() << " algos nbChunk=" << _inNbChunk);
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
	DRAIN_DEBUG("Display properties : nbAlgo : " << m_listAlgo.size());
	DRAIN_DEBUG("    Input : " << m_inputConfig);
	for (auto &it : m_listAlgo) {
		DRAIN_DEBUG("    [" << it->getType() << "] '" << it->getName() << "'");
		if (it->getInputFormat().getConfigured() == true) {
			DRAIN_DEBUG("        Input : " << it->getInputFormat());
		} else {
			DRAIN_DEBUG("        Input : Not configured");
			DRAIN_DEBUG("            format : " << it->getFormatSupportedInput());
			DRAIN_DEBUG("            frequency : " << it->getFrequencySupportedInput());
			DRAIN_DEBUG("            map : " << it->getMapSupportedInput());
		}
		if (it->getOutputFormat().getConfigured() == true) {
			DRAIN_DEBUG("        Output: " << it->getOutputFormat());
		} else {
			DRAIN_DEBUG("        Output : Not configured");
			DRAIN_DEBUG("            format : " << it->getFormatSupportedOutput());
			DRAIN_DEBUG("            frequency : " << it->getFrequencySupportedOutput());
			DRAIN_DEBUG("            map : " << it->getMapSupportedOutput());
		}
	}
	DRAIN_DEBUG("    Output : " << m_outputConfig);
	DRAIN_DEBUG("********* configuration START *************");
	// TODO : Better management of this ...
	/*
	if (m_listAlgo.size() == 0) {
		DRAIN_ERROR("manage empty drain ...");
		return;
	}
	if (m_listAlgo.size() == 1) {
		DRAIN_ERROR("manage Single drain ...");
		return;
	}
	*/
	/*
	DRAIN_INFO("configuration Input");
	if (m_listAlgo.size()>1) {
		m_listAlgo[0]->setInputFormat(m_inputConfig);
	}
	DRAIN_INFO("configuration Output");
	if (m_listAlgo.size()>1) {
		m_listAlgo[m_listAlgo.size()-1]->setOutputFormat(m_outputConfig);
	}
	*/
	for (size_t iii=0; iii<=m_listAlgo.size(); ++iii) {
		DRAIN_VERBOSE("    id = " << iii);
		if (    (    iii == 0
		          || (    iii > 0
		               && m_listAlgo[iii-1]->getOutputFormat().getConfigured() == false
		             )
		        )
		     && (    iii == m_listAlgo.size()
		          || (    iii < m_listAlgo.size()
		               && m_listAlgo[iii]->getInputFormat().getConfigured() == false
		             )
		        )
		   ) {
			// step 1 : check frequency:
			std::vector<float> freqOut;
			std::vector<float> freqIn;
			if (iii == 0) {
				freqOut.push_back(m_inputConfig.getFrequency());
			} else {
				freqOut = m_listAlgo[iii-1]->getFrequencySupportedOutput();
			}
			if (iii == m_listAlgo.size()) {
				freqIn.push_back(m_outputConfig.getFrequency());
			} else {
				freqIn = m_listAlgo[iii]->getFrequencySupportedInput();
			}
			std::vector<float> freq = getUnion<float>(freqOut, freqIn);
			DRAIN_VERBOSE("        freq out   :" << freqOut);
			DRAIN_VERBOSE("        freq in    :" << freqIn);
			DRAIN_DEBUG("        freq union :" << freq);
			
			// step 2 : Check map:
			std::vector<std::vector<audio::channel>> mapOut;
			std::vector<std::vector<audio::channel>> mapIn;
			if (iii == 0) {
				mapOut.push_back(m_inputConfig.getMap());
			} else {
				mapOut = m_listAlgo[iii-1]->getMapSupportedOutput();
			}
			if (iii == m_listAlgo.size()) {
				mapIn.push_back(m_outputConfig.getMap());
			} else {
				mapIn = m_listAlgo[iii]->getMapSupportedInput();
			}
			std::vector<std::vector<audio::channel>> map = getUnion<std::vector<audio::channel>>(mapOut, mapIn);
			DRAIN_VERBOSE("        map out   :" << mapOut);
			DRAIN_VERBOSE("        map in    :" << mapIn);
			DRAIN_DEBUG("        map union :" << map);
			// step 3 : Check Format:
			std::vector<audio::format> formatOut;
			std::vector<audio::format> formatIn;
			if (iii == 0) {
				formatOut.push_back(m_inputConfig.getFormat());
			} else {
				formatOut = m_listAlgo[iii-1]->getFormatSupportedOutput();
			}
			if (iii == m_listAlgo.size()) {
				formatIn.push_back(m_outputConfig.getFormat());
			} else {
				formatIn = m_listAlgo[iii]->getFormatSupportedInput();
			}
			std::vector<audio::format> format = getUnion<audio::format>(formatOut, formatIn);
			DRAIN_VERBOSE("        format out   :" << formatOut);
			DRAIN_VERBOSE("        format in    :" << formatIn);
			DRAIN_DEBUG("        format union :" << format);
			
			if (    freq.size() >= 1
			     && map.size() >= 1
			     && format.size() >= 1) {
				DRAIN_DEBUG("        find 1 compatibility :{format=" << format << ",frequency=" << freq << ",map=" << map << "}");
				drain::IOFormatInterface tmp(map[0], format[0], freq[0]);
				if (iii > 0) {
					m_listAlgo[iii-1]->setOutputFormat(tmp);
				}
				if (iii <m_listAlgo.size()) {
					m_listAlgo[iii]->setInputFormat(tmp);
				}
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
						if (iii == 0) {
							DRAIN_ERROR("IMPOSSIBLE CASE");
						} else {
							out.setFrequency(m_listAlgo[iii-1]->getInputFormat().getFrequency());
							in.setFrequency(m_listAlgo[iii-1]->getInputFormat().getFrequency());
						}
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
						if (iii == 0) {
							DRAIN_ERROR("IMPOSSIBLE CASE");
						} else {
							out.setMap(m_listAlgo[iii-1]->getInputFormat().getMap());
							in.setMap(m_listAlgo[iii-1]->getInputFormat().getMap());
						}
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
						if (iii == 0) {
							DRAIN_ERROR("IMPOSSIBLE CASE");
						} else {
							out.setFormat(m_listAlgo[iii-1]->getInputFormat().getFormat());
							in.setFormat(m_listAlgo[iii-1]->getInputFormat().getFormat());
						}
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
			DRAIN_DEBUG("        update: out=" << out);
			DRAIN_DEBUG("                in=" << in);
			if (iii > 0) {
				m_listAlgo[iii-1]->setOutputFormat(out);
			}
			if (iii < m_listAlgo.size()) {
				m_listAlgo[iii]->setInputFormat(in);
			}
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
					DRAIN_DEBUG("convert " << out.getFormat() << " -> " << in.getFormat());
					iii++;
				}
				// need add a resampler
				std::shared_ptr<drain::Resampler> algo = drain::Resampler::create();
				algo->setTemporary();
				algo->setInputFormat(out);
				out.setFrequency(in.getFrequency());
				algo->setOutputFormat(out);
				m_listAlgo.insert(m_listAlgo.begin()+iii, algo);
				DRAIN_DEBUG("convert " << out.getFrequency() << " -> " << in.getFrequency());
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
				DRAIN_DEBUG("convert " << out.getMap() << " -> " << in.getMap());
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
				DRAIN_DEBUG("convert " << out.getFormat() << " -> " << in.getFormat());
				iii++;
			}
			
		} else if (    (    iii > 0
		                 && m_listAlgo[iii-1]->getOutputFormat().getConfigured() == false
		               )
		            || (    iii < m_listAlgo.size()
		                 && m_listAlgo[iii]->getInputFormat().getConfigured() == false
		               )
		          ) {
			DRAIN_ERROR(" configuration error mode in " << iii-1 << " && " << iii );
		}
	}
	DRAIN_DEBUG("********* configuration will be done *************");
	DRAIN_DEBUG("    Input : " << m_inputConfig);
	for (auto &it : m_listAlgo) {
		DRAIN_DEBUG("    [" << it->getType() << "] '" << it->getName() << "'");
		if (it->getInputFormat().getConfigured() == true) {
			DRAIN_DEBUG("        Input : " << it->getInputFormat());
		} else {
			DRAIN_ERROR("        Input : Not configured");
		}
		if (it->getOutputFormat().getConfigured() == true) {
			DRAIN_DEBUG("        Output: " << it->getOutputFormat());
		} else {
			DRAIN_ERROR("        Output : Not configured");
		}
	}
	DRAIN_DEBUG("    Output : " << m_outputConfig);
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


bool drain::Process::processIn(void* _inData,
                               size_t _inNbChunk,
                               void* _outData,
                               size_t _outNbChunk){
	void* outData = nullptr;
	size_t outSize = 0;
	bool error = process(_inData, _inNbChunk, outData, outSize);
	if (outSize != _outNbChunk) {
		DRAIN_ERROR("can not copy data to output (not the same chunk number : out=" << outSize << " chunks != request=" << _outNbChunk << " chunks");
		return false;
	}
	// TODO : Do it better ...
	DRAIN_VERBOSE("Copy " << _outNbChunk << " chunks byte size=" << audio::getFormatBytes(m_outputConfig.getFormat()) << " nbChan=" << m_outputConfig.getMap().size() << " format=" << m_outputConfig.getFormat());
	memcpy(_outData, outData, _outNbChunk*audio::getFormatBytes(m_outputConfig.getFormat()) * m_outputConfig.getMap().size());
	return false;
}