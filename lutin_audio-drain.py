#!/usr/bin/python
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

def configure(target, my_module):
	
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
	    'audio/drain/debug.hpp',
	    'audio/drain/debugRemove.hpp',
	    'audio/drain/airtalgo.hpp',
	    'audio/drain/Algo.hpp',
	    'audio/drain/ChannelReorder.hpp',
	    'audio/drain/CircularBuffer.hpp',
	    'audio/drain/EndPointCallback.hpp',
	    'audio/drain/EndPoint.hpp',
	    'audio/drain/EndPointRead.hpp',
	    'audio/drain/EndPointWrite.hpp',
	    'audio/drain/FormatUpdate.hpp',
	    'audio/drain/Process.hpp',
	    'audio/drain/Resampler.hpp',
	    'audio/drain/Volume.hpp',
	    'audio/drain/IOFormatInterface.hpp',
	    'audio/drain/AutoLogInOut.hpp',
	    'audio/drain/Equalizer.hpp'
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
	my_module.add_path(".")
	return True









