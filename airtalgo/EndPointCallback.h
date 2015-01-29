/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_END_POINT_CALLBACK_H__
#define __AIRT_ALGO_END_POINT_CALLBACK_H__

#include <airtalgo/EndPoint.h>
#include <functional>


namespace airtalgo {
	typedef std::function<void (const std::chrono::system_clock::time_point& _playTime,
	                            size_t _nbChunk,
	                            const std::vector<airtalgo::channel>& _map,
	                            void* _data,
	                            enum airtalgo::format _type)> needDataFunction;
	typedef std::function<void (const std::chrono::system_clock::time_point& _readTime,
	                            size_t _nbChunk,
	                            const std::vector<airtalgo::channel>& _map,
	                            const void* _data,
	                            enum airtalgo::format _type)> haveNewDataFunction;
	class EndPointCallback : public EndPoint {
		private:
			needDataFunction m_outputFunction;
			haveNewDataFunction m_inputFunction;
		protected:
			/**
			 * @brief Constructor
			 */
			EndPointCallback();
			void init(needDataFunction _callback);
			void init(haveNewDataFunction _callback);
		public:
			static std::shared_ptr<EndPointCallback> create(needDataFunction _callback);
			static std::shared_ptr<EndPointCallback> create(haveNewDataFunction _callback);
			/**
			 * @brief Destructor
			 */
			virtual ~EndPointCallback() {};
			virtual void configurationChange();
			virtual bool process(std::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
	};
};

#endif
