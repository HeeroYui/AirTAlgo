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

#undef __class__
#define __class__ "Process"

drain::Process::Process() :
  m_isConfigured(false) {
	m_data.clear();
}
drain::Process::~Process() {
	for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
		m_listAlgo[iii].reset();
	}
}

bool drain::Process::push(std11::chrono::system_clock::time_point& _time,
                          void* _data,
                          size_t _nbChunk) {
	void* out = nullptr;
	size_t nbChunkOut;
	DRAIN_VERBOSE("        Process push");
	process(_time, _data, _nbChunk, out, nbChunkOut);
	return true;
}

bool drain::Process::pull(std11::chrono::system_clock::time_point& _time,
                          void* _data,
                          size_t _nbChunk,
                          size_t _chunkSize) {
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
		process(_time, in, nbChunkIn, out, nbChunkOut);
		if (nbChunkOut > 0) {
			size_t position = m_data.size();
			m_data.resize(m_data.size() + nbChunkOut*_chunkSize);
			memcpy(&m_data[position], out, nbChunkOut*_chunkSize);
		} else {
			// No more data in the process stream (0 input data might have flush data)
			break;
		}
	}
	// copy only data availlable :
	int32_t minByTeSize = std::min(m_data.size(), _nbChunk*_chunkSize);
	if (minByTeSize >= 0) {
		memcpy(_data, &m_data[0], minByTeSize);
		m_data.erase(m_data.begin(), m_data.begin()+minByTeSize);
	}
	return true;
}


bool drain::Process::process(std11::chrono::system_clock::time_point& _time,
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

void drain::Process::pushBack(const std11::shared_ptr<drain::Algo>& _algo) {
	removeAlgoDynamic();
	_algo->setStatusFunction(std11::bind(&drain::Process::generateStatus, this, std11::placeholders::_1, std11::placeholders::_2));
	m_listAlgo.push_back(_algo);
}

void drain::Process::pushFront(const std11::shared_ptr<drain::Algo>& _algo) {
	removeAlgoDynamic();
	_algo->setStatusFunction(std11::bind(&drain::Process::generateStatus, this, std11::placeholders::_1, std11::placeholders::_2));
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
		for (size_t ooo=0; ooo<_out.size(); ++ooo) {
			for (size_t iii=0; iii<_in.size(); ++iii) {
				if (_out[ooo] == _in[iii]) {
					out.push_back(_out[ooo]);
				}
			}
		}
	}
	return out;
}


void drain::Process::displayAlgo() {
	DRAIN_DEBUG("    Input : " << m_inputConfig);
	for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
		DRAIN_DEBUG("    [" << m_listAlgo[iii]->getType() << "] '" << m_listAlgo[iii]->getName() << "'");
		if (m_listAlgo[iii]->getInputFormat().getConfigured() == true) {
			DRAIN_DEBUG("        Input : " << m_listAlgo[iii]->getInputFormat());
		} else {
			DRAIN_DEBUG("        Input : Not configured");
			DRAIN_DEBUG("            format : " << m_listAlgo[iii]->getFormatSupportedInput());
			DRAIN_DEBUG("            frequency : " << m_listAlgo[iii]->getFrequencySupportedInput());
			DRAIN_DEBUG("            map : " << m_listAlgo[iii]->getMapSupportedInput());
		}
		if (m_listAlgo[iii]->getOutputFormat().getConfigured() == true) {
			DRAIN_DEBUG("        Output: " << m_listAlgo[iii]->getOutputFormat());
		} else {
			DRAIN_DEBUG("        Output : Not configured");
			DRAIN_DEBUG("            format : " << m_listAlgo[iii]->getFormatSupportedOutput());
			DRAIN_DEBUG("            frequency : " << m_listAlgo[iii]->getFrequencySupportedOutput());
			DRAIN_DEBUG("            map : " << m_listAlgo[iii]->getMapSupportedOutput());
		}
	}
	DRAIN_DEBUG("    Output : " << m_outputConfig);
}

void drain::Process::updateAlgo(size_t _position) {
	DRAIN_VERBOSE("    id = " << _position);
	if (    (    _position == 0
	          || (    _position > 0
	               && m_listAlgo[_position-1]->getOutputFormat().getConfigured() == false
	             )
	        )
	     && (    _position == m_listAlgo.size()
	          || (    _position < m_listAlgo.size()
	               && m_listAlgo[_position]->getInputFormat().getConfigured() == false
	             )
	        )
	   ) {
		// step 1 : check frequency:
		std::vector<float> freqOut;
		std::vector<float> freqIn;
		if (_position == 0) {
			freqOut.push_back(m_inputConfig.getFrequency());
		} else {
			freqOut = m_listAlgo[_position-1]->getFrequencySupportedOutput();
		}
		if (_position == m_listAlgo.size()) {
			freqIn.push_back(m_outputConfig.getFrequency());
		} else {
			freqIn = m_listAlgo[_position]->getFrequencySupportedInput();
		}
		std::vector<float> freq = getUnion<float>(freqOut, freqIn);
		DRAIN_VERBOSE("        freq out   :" << freqOut);
		DRAIN_VERBOSE("        freq in    :" << freqIn);
		DRAIN_VERBOSE("        freq union :" << freq);
		
		// step 2 : Check map:
		std::vector<std::vector<audio::channel> > mapOut;
		std::vector<std::vector<audio::channel> > mapIn;
		if (_position == 0) {
			mapOut.push_back(m_inputConfig.getMap());
		} else {
			mapOut = m_listAlgo[_position-1]->getMapSupportedOutput();
		}
		if (_position == m_listAlgo.size()) {
			mapIn.push_back(m_outputConfig.getMap());
		} else {
			mapIn = m_listAlgo[_position]->getMapSupportedInput();
		}
		std::vector<std::vector<audio::channel> > map = getUnion<std::vector<audio::channel> >(mapOut, mapIn);
		DRAIN_VERBOSE("        map out   :" << mapOut);
		DRAIN_VERBOSE("        map in    :" << mapIn);
		DRAIN_VERBOSE("        map union :" << map);
		// step 3 : Check Format:
		std::vector<audio::format> formatOut;
		std::vector<audio::format> formatIn;
		if (_position == 0) {
			formatOut.push_back(m_inputConfig.getFormat());
		} else {
			formatOut = m_listAlgo[_position-1]->getFormatSupportedOutput();
		}
		if (_position == m_listAlgo.size()) {
			formatIn.push_back(m_outputConfig.getFormat());
		} else {
			formatIn = m_listAlgo[_position]->getFormatSupportedInput();
		}
		std::vector<audio::format> format = getUnion<audio::format>(formatOut, formatIn);
		DRAIN_VERBOSE("        format out   :" << formatOut);
		DRAIN_VERBOSE("        format in    :" << formatIn);
		DRAIN_VERBOSE("        format union :" << format);
		
		if (    freq.size() >= 1
		     && map.size() >= 1
		     && format.size() >= 1) {
			DRAIN_VERBOSE("        find 1 compatibility :{format=" << format << ",frequency=" << freq << ",map=" << map << "}");
			drain::IOFormatInterface tmp(map[0], format[0], freq[0]);
			if (_position > 0) {
				m_listAlgo[_position-1]->setOutputFormat(tmp);
			}
			if (_position <m_listAlgo.size()) {
				m_listAlgo[_position]->setInputFormat(tmp);
			}
			return;
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
					if (_position == 0) {
						DRAIN_ERROR("IMPOSSIBLE CASE");
					} else {
						out.setFrequency(m_listAlgo[_position-1]->getInputFormat().getFrequency());
						in.setFrequency(m_listAlgo[_position-1]->getInputFormat().getFrequency());
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
					if (_position == 0) {
						DRAIN_ERROR("IMPOSSIBLE CASE");
					} else {
						out.setMap(m_listAlgo[_position-1]->getInputFormat().getMap());
						in.setMap(m_listAlgo[_position-1]->getInputFormat().getMap());
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
					if (_position == 0) {
						DRAIN_ERROR("IMPOSSIBLE CASE");
					} else {
						out.setFormat(m_listAlgo[_position-1]->getInputFormat().getFormat());
						in.setFormat(m_listAlgo[_position-1]->getInputFormat().getFormat());
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
		DRAIN_VERBOSE("        update: out=" << out);
		DRAIN_VERBOSE("                in=" << in);
		if (_position > 0) {
			m_listAlgo[_position-1]->setOutputFormat(out);
		}
		if (_position < m_listAlgo.size()) {
			m_listAlgo[_position]->setInputFormat(in);
		}
		// TODO : Add updater with an optimisation of CPU
		if (out.getFrequency() != in.getFrequency()) {
			
			// TODO : Do it better: special check for resampler : only support int16_t
			if (    out.getFormat() != audio::format_int16
			     /* && out.getFormat() != format_float */) {
				// need add a format Updater
				std11::shared_ptr<drain::FormatUpdate> algo = drain::FormatUpdate::create();
				algo->setTemporary();
				algo->setInputFormat(out);
				out.setFormat(audio::format_int16);
				algo->setOutputFormat(out);
				algo->setStatusFunction(std11::bind(&drain::Process::generateStatus, this, std11::placeholders::_1, std11::placeholders::_2));
				m_listAlgo.insert(m_listAlgo.begin()+_position, algo);
				DRAIN_VERBOSE("convert " << out.getFormat() << " -> " << in.getFormat());
				_position++;
			}
			// need add a resampler
			std11::shared_ptr<drain::Resampler> algo = drain::Resampler::create();
			algo->setTemporary();
			algo->setInputFormat(out);
			out.setFrequency(in.getFrequency());
			algo->setOutputFormat(out);
			algo->setStatusFunction(std11::bind(&drain::Process::generateStatus, this, std11::placeholders::_1, std11::placeholders::_2));
			m_listAlgo.insert(m_listAlgo.begin()+_position, algo);
			DRAIN_VERBOSE("convert " << out.getFrequency() << " -> " << in.getFrequency());
			out.setFrequency(in.getFrequency());
			_position++;
		}
		if (out.getMap() != in.getMap()) {
			// need add a channel Reorder
			std11::shared_ptr<drain::ChannelReorder> algo = drain::ChannelReorder::create();
			algo->setTemporary();
			algo->setInputFormat(out);
			out.setMap(in.getMap());
			algo->setOutputFormat(out);
			algo->setStatusFunction(std11::bind(&drain::Process::generateStatus, this, std11::placeholders::_1, std11::placeholders::_2));
			m_listAlgo.insert(m_listAlgo.begin()+_position, algo);
			DRAIN_VERBOSE("convert " << out.getMap() << " -> " << in.getMap());
			_position++;
		}
		if (out.getFormat() != in.getFormat()) {
			// need add a format Updater
			std11::shared_ptr<drain::FormatUpdate> algo = drain::FormatUpdate::create();
			algo->setTemporary();
			algo->setInputFormat(out);
			out.setFormat(in.getFormat());
			algo->setOutputFormat(out);
			algo->setStatusFunction(std11::bind(&drain::Process::generateStatus, this, std11::placeholders::_1, std11::placeholders::_2));
			m_listAlgo.insert(m_listAlgo.begin()+_position, algo);
			DRAIN_VERBOSE("convert " << out.getFormat() << " -> " << in.getFormat());
			_position++;
		}
		
	} else if (    (    _position > 0
	                 && m_listAlgo[_position-1]->getOutputFormat().getConfigured() == false
	               )
	            || (    _position < m_listAlgo.size()
	                 && m_listAlgo[_position]->getInputFormat().getConfigured() == false
	               )
	          ) {
		DRAIN_ERROR(" configuration error mode in " << _position-1 << " && " << _position );
	}
}

void drain::Process::updateInterAlgo() {
	if (m_isConfigured == true) {
		// cahin is already configured
		return ;
	}
	DRAIN_VERBOSE("Display properties : nbAlgo : " << m_listAlgo.size());
	displayAlgo();
	DRAIN_VERBOSE("********* configuration START *************");
	// configure first the endpoint ...
	if (m_listAlgo.size() > 1) {
		updateAlgo(m_listAlgo.size());
	}
	for (size_t iii=0; iii<=m_listAlgo.size(); ++iii) {
		updateAlgo(iii);
	}
	DRAIN_VERBOSE("********* configuration will be done *************");
	displayAlgo();
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

static void link(etk::FSNode& _node, const std::string& _first, const std::string& _op, const std::string& _second, bool _isLink=true) {
	if (_op == "->") {
		if (_isLink) {
			_node << "			" << _first << " -> " << _second << ";\n";
		} else {
			_node << "			" << _first << " -> " << _second << " [style=dashed];\n";
		}
	} else if (_op == "<-") {
		_node << "			" << _first << " -> " <<_second<< " [color=transparent];\n";
		if (_isLink) {
			_node << "			" << _second << " -> " << _first << " [constraint=false];\n";
		} else {
			_node << "			" << _second << " -> " << _first << " [constraint=false, style=dashed];\n";
		}
	}
}

void drain::Process::generateDot(etk::FSNode& _node, int32_t _offset, int32_t _basicID, std::string& _nameIn, std::string& _nameOut, bool _reserseGraph) {
	_node << "			subgraph clusterNode_" << _basicID << "_process {\n";
	_node << "				label=\"Drain::Process" << (_reserseGraph?"_R":"_N") << "\";\n";
	_node << "				node [shape=ellipse];\n";
	
	if (_reserseGraph == false) {
		_nameIn = "INTERFACE_ALGO_" + etk::to_string(_basicID) + "_in";
		_node << "				" << _nameIn << " [ label=\"format=" << etk::to_string(getInputConfig().getFormat())
		                                              << "\\n freq=" << getInputConfig().getFrequency()
		                                        << "\\n channelMap=" << etk::to_string(getInputConfig().getMap()) << "\\n in\" ];\n";
	} else {
		_nameIn = "INTERFACE_ALGO_" + etk::to_string(_basicID) + "_out";
		_node << "				" << _nameIn << " [ label=\"format=" << etk::to_string(getOutputConfig().getFormat())
		                                              << "\\n freq=" << getOutputConfig().getFrequency()
		                                        << "\\n channelMap=" << etk::to_string(getOutputConfig().getMap()) << "\\n out\" ];\n";
	}
	std::string connectString = _nameIn;
	_node << "				node [shape=box];\n";
	if (_reserseGraph == false) {
		for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
			if (m_listAlgo[iii] == nullptr) {
				continue;
			}
			std::string connectStringSecond = "ALGO_" + etk::to_string(_basicID) + "__" + etk::to_string(iii);
			_node << "				" << connectStringSecond << " [label=\"ALGO\\ntype='" << m_listAlgo[iii]->getType() << "'\\nname='" << m_listAlgo[iii]->getName() << "'\" ];\n";
			link(_node, connectString, "->", connectStringSecond);
			connectString = connectStringSecond;
		}
	} else {
		//for (int32_t iii=m_listAlgo.size()-1; iii>=0; --iii) {
		for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
			if (m_listAlgo[iii] == nullptr) {
				continue;
			}
			std::string connectStringSecond = "ALGO_" + etk::to_string(_basicID) + "__" + etk::to_string(iii);
			_node << "				" << connectStringSecond << " [label=\"ALGO\\ntype='" << m_listAlgo[iii]->getType() << "'\\nname='" << m_listAlgo[iii]->getName() << "'\" ];\n";
			link(_node, connectStringSecond, "<-", connectString);
			connectString = connectStringSecond;
		}
	}
	_node << "				node [shape=ellipse];\n";
	if (_reserseGraph == false) {
		_nameOut = "INTERFACE_ALGO_" + etk::to_string(_basicID) + "_out";
		_node << "				" << _nameOut << " [ label=\"format=" << etk::to_string(getOutputConfig().getFormat())
		                                               << "\\n freq=" << getOutputConfig().getFrequency()
		                                         << "\\n channelMap=" << etk::to_string(getOutputConfig().getMap()) << "\\n out\" ];\n";
	} else {
		_nameOut = "INTERFACE_ALGO_" + etk::to_string(_basicID) + "_in";
		_node << "				" << _nameOut << " [ label=\"format=" << etk::to_string(getInputConfig().getFormat())
		                                               << "\\n freq=" << getInputConfig().getFrequency()
		                                         << "\\n channelMap=" << etk::to_string(getInputConfig().getMap()) << "\\n in\" ];\n";
	}
	if (_reserseGraph == false) {
		link(_node, connectString,                                      "->", _nameOut);
	} else {
		link(_node, _nameOut, "<-", connectString);
	}
	_node << "			}\n";
}

void drain::Process::generateDotProcess(etk::FSNode& _node, int32_t _offset, int32_t _basicID, std::string& _nameIn, std::string& _nameOut, bool _reserseGraph) {
	_node << "			subgraph clusterNode_" << _basicID << "_process {\n";
	_node << "				label=\"Drain::Process" << (_reserseGraph?"_R":"_N") << "\";\n";
	_node << "				node [shape=ellipse];\n";
	
	if (_reserseGraph == true) {
		_nameIn = "INTERFACE_ALGO_" + etk::to_string(_basicID) + "_in";
		_node << "				" << _nameIn << " [ label=\"format=" << etk::to_string(getInputConfig().getFormat())
		                                              << "\\n freq=" << getInputConfig().getFrequency()
		                                        << "\\n channelMap=" << etk::to_string(getInputConfig().getMap()) << "\\n in\" ];\n";
	} else {
		_nameIn = "INTERFACE_ALGO_" + etk::to_string(_basicID) + "_out";
		_node << "				" << _nameIn << " [ label=\"format=" << etk::to_string(getOutputConfig().getFormat())
		                                              << "\\n freq=" << getOutputConfig().getFrequency()
		                                        << "\\n channelMap=" << etk::to_string(getOutputConfig().getMap()) << "\\n out\" ];\n";
	}
	std::string connectString = _nameIn;
	_node << "				node [shape=box];\n";
	if (_reserseGraph == false) {
		for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
			if (m_listAlgo[iii] == nullptr) {
				continue;
			}
			std::string connectStringSecond = "ALGO_" + etk::to_string(_basicID) + "__" + etk::to_string(iii);
			_node << "				" << connectStringSecond << " [label=\"ALGO\\ntype='" << m_listAlgo[iii]->getType() << "'\\nname='" << m_listAlgo[iii]->getName() << "'\" ];\n";
			link(_node, connectString, "->", connectStringSecond);
			connectString = connectStringSecond;
		}
	} else {
		//for (int32_t iii=m_listAlgo.size()-1; iii>=0; --iii) {
		for (size_t iii=0; iii<m_listAlgo.size(); ++iii) {
			if (m_listAlgo[iii] == nullptr) {
				continue;
			}
			std::string connectStringSecond = "ALGO_" + etk::to_string(_basicID) + "__" + etk::to_string(iii);
			_node << "				" << connectStringSecond << " [label=\"ALGO\\ntype='" << m_listAlgo[iii]->getType() << "'\\nname='" << m_listAlgo[iii]->getName() << "'\" ];\n";
			link(_node, connectStringSecond, "<-", connectString);
			connectString = connectStringSecond;
		}
	}
	_node << "				node [shape=ellipse];\n";
	if (_reserseGraph == true) {
		_nameOut = "INTERFACE_ALGO_" + etk::to_string(_basicID) + "_out";
		_node << "				" << _nameOut << " [ label=\"format=" << etk::to_string(getOutputConfig().getFormat())
		                                               << "\\n freq=" << getOutputConfig().getFrequency()
		                                         << "\\n channelMap=" << etk::to_string(getOutputConfig().getMap()) << "\\n out\" ];\n";
	} else {
		_nameOut = "INTERFACE_ALGO_" + etk::to_string(_basicID) + "_in";
		_node << "				" << _nameOut << " [ label=\"format=" << etk::to_string(getInputConfig().getFormat())
		                                               << "\\n freq=" << getInputConfig().getFrequency()
		                                         << "\\n channelMap=" << etk::to_string(getInputConfig().getMap()) << "\\n in\" ];\n";
	}
	if (_reserseGraph == false) {
		link(_node, connectString,                                      "->", _nameOut);
	} else {
		link(_node, _nameOut, "<-", connectString);
	}
	_node << "			}\n";
}


void drain::Process::generateStatus(const std::string& _origin, const std::string& _status) {
	if (m_statusFunction != nullptr) {
		m_statusFunction(_origin, _status);
	}
}

void drain::Process::setStatusFunction(statusFunction _newFunction) {
	m_statusFunction = _newFunction;
}
