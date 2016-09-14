#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug


def get_type():
	return "LIBRARY"

def get_desc():
	return "Basic audio algo interface single pipe to provess data"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def get_version():
	return "version.txt"

def create(target, module_name):
	my_module = module.Module(__file__, module_name, get_type())
	
	my_module.add_src_file([
		'audio/drain/debug.cpp',
		'audio/drain/airtalgo.cpp',
		'audio/drain/Algo.cpp',
		'audio/drain/ChannelReorder.cpp',
		'audio/drain/CircularBuffer.cpp',
		'audio/drain/EndPointCallback.cpp',
		'audio/drain/EndPoint.cpp',
		'audio/drain/EndPointRead.cpp',
		'audio/drain/EndPointWrite.cpp',
		'audio/drain/FormatUpdate.cpp',
		'audio/drain/Process.cpp',
		'audio/drain/Resampler.cpp',
		'audio/drain/Volume.cpp',
		'audio/drain/IOFormatInterface.cpp',
		'audio/drain/AutoLogInOut.cpp',
		'audio/drain/Equalizer.cpp'
		])
	
	my_module.add_header_file([
		'audio/drain/debug.h',
		'audio/drain/debugRemove.h',
		'audio/drain/airtalgo.h',
		'audio/drain/Algo.h',
		'audio/drain/ChannelReorder.h',
		'audio/drain/CircularBuffer.h',
		'audio/drain/EndPointCallback.h',
		'audio/drain/EndPoint.h',
		'audio/drain/EndPointRead.h',
		'audio/drain/EndPointWrite.h',
		'audio/drain/FormatUpdate.h',
		'audio/drain/Process.h',
		'audio/drain/Resampler.h',
		'audio/drain/Volume.h',
		'audio/drain/IOFormatInterface.h',
		'audio/drain/AutoLogInOut.h',
		'audio/drain/Equalizer.h'
		])
	
	# TODO: my_module.add_optional_module_depend('speexdsp', "HAVE_SPEEX_DSP_RESAMPLE")
	my_module.add_flag('c++', "-DHAVE_SPEEX_DSP_RESAMPLE")
	my_module.add_depend([
	    'etk',
	    'audio',
	    'ejson',
	    'speex-dsp',
	    'audio-algo-drain'
	    ])
	my_module.add_path(tools.get_current_path(__file__))
	return my_module









