#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "single audio flow test"


def create(target):
	myModule = module.Module(__file__, 'audio-drain-test', 'BINARY')
	
	myModule.add_src_file([
		'test/main.cpp',
		'test/debug.cpp',
		'test/updateFlow.cpp',
		'test/resampling.cpp',
		'test/format.cpp',
		'test/channelOrder.cpp',
		'test/equalizer.cpp'
		])
	
	myModule.add_module_depend(['audio-drain', 'gtest', 'etk'])
	
	return myModule









