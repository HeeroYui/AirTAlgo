/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __DRAIN_ALGO_BIQUAD_FLOAT_H__
#define __DRAIN_ALGO_BIQUAD_FLOAT_H__

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
	class BiQuadFloat {
		public:
			BiQuadFloat();
		protected:
			float m_x[2]; //!< X history
			float m_y[2]; //!< Y histiry
			float m_a[3]; //!< A bi-Quad coef
			float m_b[2]; //!< B bi-Quad coef
		public:
			/**
			 * @brief Set the bi-quad value and type
			 * @param[in] _type Type of biquad.
			 * @param[in] _frequencyCut Cut Frequency. [0..sampleRate/2]
			 * @param[in] _qualityFactor Q factor of quality (good value of 0.707 ==> permit to not ower gain) limit [0.01 .. 10]
			 * @param[in] _gain Gain to apply (for notch, peak, lowShelf and highShelf) limit : -30, +30
			 * @param[in] _sampleRate Sample rate of the signal
			 */
			void setBiquad(enum drain::filterType _type, double _frequencyCut, double _qualityFactor, double _gain, float _sampleRate);
			/**
			 * @brief Set direct Coefficients
			 */
			void setBiquadCoef(float _a0, float _a1, float _a2, float _b0, float _b1);
			/**
			 * @brief Get direct Coefficients
			 */
			void getBiquadCoef(float& _a0, float& _a1, float& _a2, float& _b0, float& _b1);
			/**
			 * @brief Get direct Coefficients
			 */
			std::vector<float> getCoef();
			/**
			 * @brief Reset bequad filter (only history not value).
			 */
			void reset();
		protected:
			/**
			 * @brief process single sample in float.
			 * @param[in] _sample Sample to process
			 * @return updataed value
			 */
			float processFloat(float _sample);
		public:
			/**
			 * @brief Porcess function.
			 * param[in] _input Pointer on the input data.
			 * param[in,out] _output Poirter on the output data (can be the same as input (inplace availlable).
			 * param[in] _nbChunk Number of qample to process.
			 * param[in] _inputOffset Offset to add when read input data.
			 * param[in] _outputOffset Offset to add when write output data.
			 */
			void processFloat(float* _input,
			                  float* _output,
			                  size_t _nbChunk,
			                  int32_t _inputOffset,
			                  int32_t _outputOffset);
			//! @previous
			void processInt16(int16_t* _input,
			                  int16_t* _output,
			                  size_t _nbChunk,
			                  int32_t _inputOffset,
			                  int32_t _outputOffset);
			/**
			 * @brief calculate respond of the filter:
			 * @param[in] _sampleRate input qample rate
			 * @retrun list of frequency/power in dB
			 */
			std::vector<std::pair<float,float> > calculateTheory(double _sampleRate);
	};
}

#endif

