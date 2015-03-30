#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "drain : Basic audio algo interface single pipe to provess data"


def create(target):
	myModule = module.Module(__file__, 'drain', 'LIBRARY')
	
	myModule.add_src_file([
		'drain/debug.cpp',
		'drain/airtalgo.cpp',
		'drain/Algo.cpp',
		'drain/BiQuadFloat.cpp',
		'drain/ChannelReorder.cpp',
		'drain/CircularBuffer.cpp',
		'drain/EndPointCallback.cpp',
		'drain/EndPoint.cpp',
		'drain/EndPointRead.cpp',
		'drain/EndPointWrite.cpp',
		'drain/FormatUpdate.cpp',
		'drain/Process.cpp',
		'drain/Resampler.cpp',
		'drain/Volume.cpp',
		'drain/IOFormatInterface.cpp',
		'drain/AutoLogInOut.cpp',
		'drain/Equalizer.cpp',
		'drain/echoCanceller/Lms.cpp'
		])
	
	# TODO: myModule.add_optional_module_depend('speexdsp', "HAVE_SPEEX_DSP_RESAMPLE")
	myModule.compile_flags_CC("-DHAVE_SPEEX_DSP_RESAMPLE")
	myModule.add_module_depend(['etk', 'audio', 'ejson', 'speexdsp'])
	myModule.add_export_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return myModule









