#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools


# optionnal : Describe in the "lutin.py --help"
def get_desc():
	return "drain_equalizer_profiling : basic test and profiling of equalizer work or not"

def create(target):
	my_module = module.Module(__file__, 'drain_equalizer_profiling', 'BINARY')
	# add the file to compile:
	my_module.add_src_file([
		'appl/main.cpp',
		'appl/debug.cpp',
		'appl/Windows.cpp',
		'appl/widget/DisplayFrequency.cpp',
		])
	# add Library dependency name
	my_module.add_module_depend(['ewol', 'audio-drain', 'audio-river'])
	# add application C flags
	my_module.compile_flags('c++', [
		"-DPROJECT_NAME=\"\\\""+my_module.name+"\\\"\""])
	# Add current include Path
	my_module.add_path(tools.get_current_path(__file__))
	# copy internal datas
	my_module.copy_folder("data/*")
	
	# return the created module
	return my_module




