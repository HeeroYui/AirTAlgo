#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "drain_test : single audio flow test"


def create(target):
	myModule = module.Module(__file__, 'drain_test', 'BINARY')
	
	myModule.add_src_file([
		'test/main.cpp',
		'test/debug.cpp',
		'test/updateFlow.cpp',
		'test/resampling.cpp',
		'test/format.cpp',
		'test/channelOrder.cpp',
		'test/equalizer.cpp'
		])
	
	myModule.add_module_depend(['drain', 'gtest', 'etk'])
	
	return myModule









