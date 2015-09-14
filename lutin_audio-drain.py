#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "Basic audio algo interface single pipe to provess data"


def create(target):
	myModule = module.Module(__file__, 'audio-drain', 'LIBRARY')
	
	myModule.add_src_file([
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
	
	myModule.add_header_file([
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
	
	# TODO: myModule.add_optional_module_depend('speexdsp', "HAVE_SPEEX_DSP_RESAMPLE")
	myModule.compile_flags('c++', "-DHAVE_SPEEX_DSP_RESAMPLE")
	myModule.add_module_depend(['etk', 'audio', 'ejson', 'speex-dsp', 'audio-algo-drain'])
	myModule.add_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return myModule









