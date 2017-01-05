/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <audio/drain/Algo.hpp>

namespace audio {
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
				virtual ~EndPoint() {
					
				};
		};
	}
}

