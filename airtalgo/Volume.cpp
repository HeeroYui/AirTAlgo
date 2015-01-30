/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"
#include <airtalgo/Volume.h>
#include <iostream>

#undef __class__
#define __class__ "Volume"

airtalgo::Volume::Volume() :
  m_volume(0.0f),
  m_volumeAppli(1.0f) {
	
}

void airtalgo::Volume::init() {
	airtalgo::Algo::init();
}

std::shared_ptr<airtalgo::Volume> airtalgo::Volume::create() {
	std::shared_ptr<airtalgo::Volume> tmp(new airtalgo::Volume());
	tmp->init();
	return tmp;
}

airtalgo::Volume::~Volume() {
	
}

void airtalgo::Volume::configurationChange() {
	airtalgo::Algo::configurationChange();
	if (m_input.getFormat() != m_output.getFormat()) {
		AIRTALGO_ERROR("Volume format change is not supported");
	}
	if (m_input.getMap() != m_output.getMap()) {
		AIRTALGO_ERROR("Volume map change is not supported");
	}
	if (m_input.getFrequency() != m_output.getFrequency()) {
		AIRTALGO_ERROR("Volume frequency change is not supported");
	}
	// nee to process all time (the format not change (just a simple filter))
	m_needProcess = false;
}

bool airtalgo::Volume::process(std::chrono::system_clock::time_point& _time,
                                  void* _input,
                                  size_t _inputNbChunk,
                                  void*& _output,
                                  size_t& _outputNbChunk) {
	airtalgo::autoLogInOut tmpLog("Volume");
	if (m_volumeAppli == 1.0f) {
		_output = _input;
		_outputNbChunk = _inputNbChunk;
		return true;
	}
	
}
