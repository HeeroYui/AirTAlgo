#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "audio_drain : Basic audio algo interface single pipe to provess data"


def create(target):
	myModule = module.Module(__file__, 'audio_drain', 'LIBRARY')
	
	myModule.add_src_file([
		'audio/drain/debug.cpp',
		'audio/drain/airtalgo.cpp',
		'audio/drain/Algo.cpp',
		'audio/drain/BiQuadFloat.cpp',
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
	
	# TODO: myModule.add_optional_module_depend('speexdsp', "HAVE_SPEEX_DSP_RESAMPLE")
	myModule.compile_flags_CC("-DHAVE_SPEEX_DSP_RESAMPLE")
	myModule.add_module_depend(['etk', 'audio', 'ejson', 'speexdsp'])
	myModule.add_export_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return myModule









