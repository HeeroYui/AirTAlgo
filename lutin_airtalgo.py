#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "airtalgo : Basic algo interface"


def create(target):
	myModule = module.Module(__file__, 'airtalgo', 'LIBRARY')
	
	myModule.add_src_file([
		'airtalgo/debug.cpp',
		'airtalgo/airtalgo.cpp',
		'airtalgo/Algo.cpp',
		'airtalgo/channel.cpp',
		'airtalgo/ChannelReorder.cpp',
		'airtalgo/EndPointCallback.cpp',
		'airtalgo/EndPoint.cpp',
		'airtalgo/EndPointRead.cpp',
		'airtalgo/EndPointWrite.cpp',
		'airtalgo/format.cpp',
		'airtalgo/FormatUpdate.cpp',
		'airtalgo/Process.cpp',
		'airtalgo/Resampler.cpp'
		])
	
	# TODO: myModule.add_optionnal_module_depend('speexdsp', "HAVE_SPEEX_DSP_RESAMPLE")
	myModule.compile_flags_CC("-DHAVE_SPEEX_DSP_RESAMPLE")
	myModule.add_module_depend(['etk', 'speexdsp'])
	myModule.add_export_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return myModule









