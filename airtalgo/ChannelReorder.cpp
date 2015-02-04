/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <airtalgo/ChannelReorder.h>
#include <iostream>
#include "debug.h"
#include "debug.h"


#undef __class__
#define __class__ "ChannelReorder"

airtalgo::ChannelReorder::ChannelReorder() {
	
}


void airtalgo::ChannelReorder::init() {
	airtalgo::Algo::init();
	m_type = "ChannelReorder";
}

std::shared_ptr<airtalgo::ChannelReorder> airtalgo::ChannelReorder::create() {
	std::shared_ptr<airtalgo::ChannelReorder> tmp(new airtalgo::ChannelReorder());
	tmp->init();
	return tmp;
}

void airtalgo::ChannelReorder::configurationChange() {
	airtalgo::AutoLogInOut("ChannelReorder (config)");
	airtalgo::Algo::configurationChange();
	if (m_input.getFormat() != m_output.getFormat()) {
		AIRTALGO_ERROR("can not support Format Change ...");
		m_needProcess = false;
	}
	if (m_input.getFrequency() != m_output.getFrequency()) {
		AIRTALGO_ERROR("can not support frequency Change ...");
		m_needProcess = false;
	}
	if (m_input.getMap() == m_output.getMap()) {
		// nothing to process...
		m_needProcess = false;
		AIRTALGO_INFO(" no need to convert ... " << m_input.getMap() << " ==> " << m_output.getMap());
		return;
	}
}


bool airtalgo::ChannelReorder::process(std::chrono::system_clock::time_point& _time,
                                              void* _input,
                                              size_t _inputNbChunk,
                                              void*& _output,
                                              size_t& _outputNbChunk) {
	airtalgo::AutoLogInOut tmpLog("ChannelReorder");
	_outputNbChunk = _inputNbChunk;
	// check if we need to process:
	if (m_needProcess == false) {
		_output = _input;
		return true;
	}
	if (_input == nullptr) {
		_output = &(m_outputData[0]);
		_outputNbChunk = 0;
		AIRTALGO_ERROR("null pointer input ... ");
		return false;
	}
	m_outputData.resize(_outputNbChunk*m_output.getMap().size()*m_formatSize);
	_output = &(m_outputData[0]);
	// real process: (only depend of data size):
	switch (m_output.getFormat()) {
		case audio::format_int8:
			{
				AIRTALGO_VERBOSE("convert " << m_input.getMap() << " ==> " << m_output.getMap() << " format=" << int32_t(m_formatSize));
				int8_t* in = static_cast<int8_t*>(_input);
				int8_t* out = static_cast<int8_t*>(_output);
				for (size_t kkk=0; kkk<m_output.getMap().size(); ++kkk) {
					int32_t convertId = -1;
					if (    m_input.getMap().size() == 1
					     && m_input.getMap()[0] == audio::channel_frontCenter) {
						convertId = 0;
					} else {
						for (size_t jjj=0; jjj<m_input.getMap().size(); ++jjj) {
							if (m_output.getMap()[kkk] == m_input.getMap()[jjj]) {
								convertId = jjj;
								break;
							}
						}
					}
					AIRTALGO_VERBOSE("    " << convertId << " ==> " << kkk);
					if (convertId == -1) {
						for (size_t iii=0; iii<_outputNbChunk; ++iii) {
							out[iii*m_output.getMap().size()+kkk] = 0;
						}
					} else {
						for (size_t iii=0; iii<_outputNbChunk; ++iii) {
							out[iii*m_output.getMap().size()+kkk] = in[iii*m_input.getMap().size()+convertId];
						}
					}
				}
			}
			break;
		default:
		case audio::format_int16:
			{
				AIRTALGO_VERBOSE("convert " << m_input.getMap() << " ==> " << m_output.getMap() << " format=" << int32_t(m_formatSize));
				int16_t* in = static_cast<int16_t*>(_input);
				int16_t* out = static_cast<int16_t*>(_output);
				for (size_t kkk=0; kkk<m_output.getMap().size(); ++kkk) {
					int32_t convertId = -1;
					if (    m_input.getMap().size() == 1
					     && m_input.getMap()[0] == audio::channel_frontCenter) {
						convertId = 0;
					} else {
						for (size_t jjj=0; jjj<m_input.getMap().size(); ++jjj) {
							if (m_output.getMap()[kkk] == m_input.getMap()[jjj]) {
								convertId = jjj;
								break;
							}
						}
					}
					AIRTALGO_VERBOSE("    " << convertId << " ==> " << kkk);
					if (convertId == -1) {
						for (size_t iii=0; iii<_outputNbChunk; ++iii) {
							out[iii*m_output.getMap().size()+kkk] = 0;
						}
					} else {
						for (size_t iii=0; iii<_outputNbChunk; ++iii) {
							out[iii*m_output.getMap().size()+kkk] = in[iii*m_input.getMap().size()+convertId];
						}
					}
				}
			}
			break;
		case audio::format_int16_on_int32:
		case audio::format_int24:
		case audio::format_int32:
		case audio::format_float:
			{
				AIRTALGO_VERBOSE("convert (2) " << m_input.getMap() << " ==> " << m_output.getMap());
				uint32_t* in = static_cast<uint32_t*>(_input);
				uint32_t* out = static_cast<uint32_t*>(_output);
				for (size_t kkk=0; kkk<m_output.getMap().size(); ++kkk) {
					int32_t convertId = -1;
					if (    m_input.getMap().size() == 1
					     && m_input.getMap()[0] == audio::channel_frontCenter) {
						convertId = 0;
					} else {
						for (size_t jjj=0; jjj<m_input.getMap().size(); ++jjj) {
							if (m_output.getMap()[kkk] == m_input.getMap()[jjj]) {
								convertId = jjj;
								break;
							}
						}
					}
					if (convertId == -1) {
						for (size_t iii=0; iii<_outputNbChunk; ++iii) {
							out[iii*m_output.getMap().size()+kkk] = 0;
						}
					} else {
						for (size_t iii=0; iii<_outputNbChunk; ++iii) {
							out[iii*m_output.getMap().size()+kkk] = in[iii*m_input.getMap().size()+convertId];
						}
					}
				}
			}
			break;
		case audio::format_double:
			{
				AIRTALGO_VERBOSE("convert (2) " << m_input.getMap() << " ==> " << m_output.getMap());
				uint64_t* in = static_cast<uint64_t*>(_input);
				uint64_t* out = static_cast<uint64_t*>(_output);
				for (size_t kkk=0; kkk<m_output.getMap().size(); ++kkk) {
					int32_t convertId = -1;
					if (    m_input.getMap().size() == 1
					     && m_input.getMap()[0] == audio::channel_frontCenter) {
						convertId = 0;
					} else {
						for (size_t jjj=0; jjj<m_input.getMap().size(); ++jjj) {
							if (m_output.getMap()[kkk] == m_input.getMap()[jjj]) {
								convertId = jjj;
								break;
							}
						}
					}
					if (convertId == -1) {
						for (size_t iii=0; iii<_outputNbChunk; ++iii) {
							out[iii*m_output.getMap().size()+kkk] = 0;
						}
					} else {
						for (size_t iii=0; iii<_outputNbChunk; ++iii) {
							out[iii*m_output.getMap().size()+kkk] = in[iii*m_input.getMap().size()+convertId];
						}
					}
				}
			}
			break;
	}
	return true;
}
