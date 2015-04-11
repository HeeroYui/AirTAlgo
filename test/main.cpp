/** @file
 * @author Edouard DUPIN 
 * @copyright 2015, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <etk/etk.h>
#include <test/debug.h>
#include <gtest/gtest.h>


#undef __class__
#define __class__ "test"

int main(int _argc, const char** _argv) {
	// init Google test :
	::testing::InitGoogleTest(&_argc, const_cast<char **>(_argv));
	// the only one init for etk:
	etk::init(_argc, _argv);
	for (int32_t iii=0; iii<_argc ; ++iii) {
		std::string data = _argv[iii];
		if (    data == "-h"
		     || data == "--help") {
			APPL_INFO("Help : ");
			APPL_INFO("    ./xxx ---");
			exit(0);
		}
	}
	return RUN_ALL_TESTS();
}

