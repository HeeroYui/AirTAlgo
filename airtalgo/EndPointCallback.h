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
	enum formatDataType {
		formatDataTypeInt16,
		formatDataTypeInt32,
	};
	typedef std::function<void (const std::chrono::system_clock::time_point& _playTime, size_t _nbChunk, const std::vector<airtalgo::channel>& _map, void* _data, enum formatDataType& _type)> needDataFunction;
	typedef std::function<void (const std::chrono::system_clock::time_point& _readTime, size_t _nbChunk, const std::vector<airtalgo::channel>& _map, const void* _data, enum formatDataType& _type)> haveNewDataFunction;
	class EndPointCallback : public EndPoint {
		private:
			enum formatDataType m_dataFormat;
			needDataFunction m_output;
			haveNewDataFunction m_input;
			std::vector<uint8_t> m_data;
		public:
			/**
			 * @brief Constructor
			 */
			EndPointCallback(needDataFunction _callback, enum formatDataType _dataFormat);
			EndPointCallback(haveNewDataFunction _callback, enum formatDataType _dataFormat);
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
