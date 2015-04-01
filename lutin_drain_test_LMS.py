#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "drain_test_LMS : Specific designed test for LMS ALGO"


def create(target):
	myModule = module.Module(__file__, 'drain_test_LMS', 'BINARY')
	
	myModule.add_src_file([
		'testLMS/main.cpp',
		'testLMS/debug.cpp'
		])
	
	myModule.add_module_depend(['drain', 'etk'])
	
	return myModule









