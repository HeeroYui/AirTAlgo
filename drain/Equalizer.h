/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __DRAIN_ALGO_EQUALIZER_H__
#define __DRAIN_ALGO_EQUALIZER_H__

#include <drain/Algo.h>
#include <etk/memory.h>

namespace drain {
	enum filterType {
		filterType_none, //!< no filter
		filterType_LPF, //!< low pass filter
		filterType_HPF, //!< High pass filter
		filterType_BPF, //!< band pass filter
		filterType_NOTCH, //!< Notch Filter
		filterType_APF, //!< All pass Filter
		filterType_PeakingEQ, //!< Peaking band EQ filter
		filterType_LSH, //!< Low shelf filter
		filterType_HSH, //!< High shelf filter 
		filterType_EQU //!< Equalizer
	};
	class BGHistory {
		public:
			BGHistory() {
				m_x[0] = 0;
				m_y[1] = 0;
				m_x[0] = 0;
				m_y[1] = 0;
			}
			float m_x[2]; //!< X history
			float m_y[2]; //!< Y histiry
	};
	class Equalizer : public Algo {
		protected:
			/**
			 * @brief Constructor
			 */
			Equalizer();
			void init();
		public:
			static std11::shared_ptr<Equalizer> create();
			/**
			 * @brief Destructor
			 */
			virtual ~Equalizer();
		protected:
			virtual void configurationChange();
		public:
			virtual bool process(std11::chrono::system_clock::time_point& _time,
			                     void* _input,
			                     size_t _inputNbChunk,
			                     void*& _output,
			                     size_t& _outputNbChunk);
			virtual bool setParameter(const std::string& _parameter, const std::string& _value);
			virtual std::string getParameter(const std::string& _parameter) const;
			virtual std::string getParameterProperty(const std::string& _parameter) const;
			
		protected:
			float processFloat(float _sample, drain::BGHistory& _history);
			//-----------------------------------------
			// START parameters:
			enum filterType m_type; //!< current filter type.
			float m_gain; //!< Gain to apply in dB
			float m_frequency; //!< Frequency to apply filter
			float m_bandWidth; //!< Band With to apply filter
			// END parameters:
			//-----------------------------------------
			float m_a[2]; //!< A bi-Quad coef
			float m_b[3]; //!< B bi-Quad coef
			std::vector<BGHistory> m_history;
			/**
			 * @brief Configure the current biquad.
			 */
			bool configureBiQuad();
	};
};

#endif
