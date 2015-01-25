/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRTALGO_DEBUG_H__
#define __AIRTALGO_DEBUG_H__

#include <etk/log.h>

namespace airtalgo {
	int32_t getLogId();
};
// TODO : Review this problem of multiple intanciation of "std::stringbuf sb"
#define AIRTALGO_BASE(info,data) \
	do { \
		if (info <= etk::log::getLevel(airtalgo::getLogId())) { \
			std::stringbuf sb; \
			std::ostream tmpStream(&sb); \
			tmpStream << data; \
			etk::log::logStream(airtalgo::getLogId(), info, __LINE__, __class__, __func__, tmpStream); \
		} \
	} while(0)

#define AIRTALGO_CRITICAL(data)      AIRTALGO_BASE(1, data)
#define AIRTALGO_ERROR(data)         AIRTALGO_BASE(2, data)
#define AIRTALGO_WARNING(data)       AIRTALGO_BASE(3, data)
#ifdef DEBUG
	#define AIRTALGO_INFO(data)          AIRTALGO_BASE(4, data)
	#define AIRTALGO_DEBUG(data)         AIRTALGO_BASE(5, data)
	#define AIRTALGO_VERBOSE(data)       AIRTALGO_BASE(6, data)
	#define AIRTALGO_TODO(data)          AIRTALGO_BASE(4, "TODO : " << data)
#else
	#define AIRTALGO_INFO(data)          do { } while(false)
	#define AIRTALGO_DEBUG(data)         do { } while(false)
	#define AIRTALGO_VERBOSE(data)       do { } while(false)
	#define AIRTALGO_TODO(data)          do { } while(false)
#endif

#define AIRTALGO_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			AIRTALGO_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)

#endif

