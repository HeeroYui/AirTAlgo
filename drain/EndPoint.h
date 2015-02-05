/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_END_POINT_H__
#define __AIRT_ALGO_END_POINT_H__

#include <drain/Algo.h>

namespace drain{
	class EndPoint : public Algo {
		protected:
			/**
			 * @brief Constructor
			 */
			EndPoint() {};
			void init() {
				drain::Algo::init();
			};
		public:
			/**
			 * @brief Destructor
			 */
			virtual ~EndPoint() {};
	};
};

#endif
