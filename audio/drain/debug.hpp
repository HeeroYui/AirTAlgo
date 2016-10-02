/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AUDIO_DRAIN_DEBUG_H__
#define __AUDIO_DRAIN_DEBUG_H__

#include <elog/log.hpp>

namespace audio {
	namespace drain {
		int32_t getLogId();
	}
}
#define DRAIN_BASE(info,data) ELOG_BASE(audio::drain::getLogId(),info,data)

#define DRAIN_CRITICAL(data)      DRAIN_BASE(1, data)
#define DRAIN_ERROR(data)         DRAIN_BASE(2, data)
#define DRAIN_WARNING(data)       DRAIN_BASE(3, data)
#ifdef DEBUG
	#define DRAIN_INFO(data)          DRAIN_BASE(4, data)
	#define DRAIN_DEBUG(data)         DRAIN_BASE(5, data)
	#define DRAIN_VERBOSE(data)       DRAIN_BASE(6, data)
	#define DRAIN_TODO(data)          DRAIN_BASE(4, "TODO : " << data)
#else
	#define DRAIN_INFO(data)          do { } while(false)
	#define DRAIN_DEBUG(data)         do { } while(false)
	#define DRAIN_VERBOSE(data)       do { } while(false)
	#define DRAIN_TODO(data)          do { } while(false)
#endif

#define DRAIN_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			DRAIN_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)

#endif

