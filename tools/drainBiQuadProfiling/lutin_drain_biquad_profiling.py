#!/usr/bin/python
import lutin.debug as debug
import lutin.tools as tools


def get_type():
	return "BINARY"

def get_sub_type():
	return "TOOLS"

def get_desc():
	return "basic test and profiling of equalizer work or not"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return ["Mr DUPIN Edouard <yui.heero@gmail.com>"]

def configure(target, my_module):
	my_module.add_src_file([
	    'appl/main.cpp',
	    'appl/debug.cpp',
	    'appl/Windows.cpp',
	    'appl/widget/DisplayFrequency.cpp',
	    ])
	my_module.add_depend([
	    'ewol',
	    'audio-drain',
	    'audio-river'
	    ])
	my_module.add_flag('c++', [
	    "-DPROJECT_NAME=\"\\\""+my_module.get_name()+"\\\"\""
	    ])
	my_module.add_path(".")
	my_module.copy_path("data/*")
	return True




