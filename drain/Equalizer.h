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
		filterType_none, //!< no filter (pass threw...)
		filterType_lowPass, //!< low pass filter
		filterType_highPass, //!< High pass filter
		filterType_bandPass, //!< band pass filter
		filterType_notch, //!< Notch Filter
		filterType_peak, //!< Peaking band EQ filter
		filterType_lowShelf, //!< Low shelf filter
		filterType_highShelf, //!< High shelf filter
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
			float m_gain; //!< Gain to apply in dB  ??? limit : -30, +30
			float m_frequencyCut; //!< Frequency to apply filter ???? LIMIT : [0..sampleRate/2]
			// good value of 0.707 ==> permit to not ower gain
			float m_qualityFactor; //!< Quality factor ??? limit [0.01 .. 10]
			// END parameters:
			//-----------------------------------------
			float m_a[3]; //!< A bi-Quad coef
			float m_b[2]; //!< B bi-Quad coef
			std::vector<BGHistory> m_history;
			/**
			 * @brief Configure the current biquad.
			 */
			bool configureBiQuad();
		public:
			/**
			 * @brief Configure the current biquad.
			 */
			void calcBiquad(enum drain::filterType _type, double _frequencyCut, double _qualityFactor, double _gain);
			std::vector<float> getCoef() {
				std::vector<float> out;
				out.push_back(m_a[0]);
				out.push_back(m_a[1]);
				out.push_back(m_a[2]);
				out.push_back(m_b[0]);
				out.push_back(m_b[1]);
				return out;
			}
	};
};

#endif
