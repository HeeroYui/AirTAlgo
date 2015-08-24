#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools


# optionnal : Describe in the "lutin.py --help"
def get_desc():
	return "drain_biquad_profiling : basic test and profiling of equalizer work or not"

def create(target):
	myModule = module.Module(__file__, 'drain_biquad_profiling', 'BINARY')
	# add the file to compile:
	myModule.add_src_file([
		'appl/main.cpp',
		'appl/debug.cpp',
		'appl/Windows.cpp',
		'appl/widget/DisplayFrequency.cpp',
		])
	# add Library dependency name
	myModule.add_module_depend(['ewol', 'audio-drain', 'audio-river'])
	# add application C flags
	myModule.compile_flags('c++', [
		"-DPROJECT_NAME=\"\\\""+myModule.name+"\\\"\""])
	# Add current include Path
	myModule.add_path(tools.get_current_path(__file__))
	# copy internal datas
	myModule.copy_folder("data/*")
	
	# return the created module
	return myModule




