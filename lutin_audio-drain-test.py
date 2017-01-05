#!/usr/bin/python
import lutin.debug as debug
import lutin.tools as tools


def get_type():
	return "BINARY"

def get_sub_type():
	return "TEST"

def get_desc():
	return "single audio flow test"

def get_licence():
	return "MPL-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def configure(target, my_module):
	my_module.add_src_file([
		'test/main.cpp',
		'test/updateFlow.cpp',
		'test/resampling.cpp',
		'test/format.cpp',
		'test/channelOrder.cpp',
		'test/equalizer.cpp'
		])
	my_module.add_depend([
	    'audio-drain',
	    'gtest',
	    'etk',
	    'test-debug'
	    ])
	return True









