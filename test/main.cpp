/** @file
 * @author Edouard DUPIN 
 * @copyright 2015, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/etk.hpp>
#include <test-debug/debug.hpp>
#include <gtest/gtest.h>

int main(int _argc, const char** _argv) {
	// init Google test :
	::testing::InitGoogleTest(&_argc, const_cast<char **>(_argv));
	// the only one init for etk:
	etk::init(_argc, _argv);
	for (int32_t iii=0; iii<_argc ; ++iii) {
		std::string data = _argv[iii];
		if (    data == "-h"
		     || data == "--help") {
			TEST_INFO("Help : ");
			TEST_INFO("    ./xxx ---");
			exit(0);
		}
	}
	return RUN_ALL_TESTS();
}

