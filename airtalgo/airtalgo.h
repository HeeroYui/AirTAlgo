/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AIRT_ALGO_CORE_H__
#define __AIRT_ALGO_CORE_H__

#include <string>
#include <airtalgo/format.h>
#include <airtalgo/channel.h>
#include <chrono>

namespace airtalgo {
	/**
	 * @brief Init the instance af algorithm (add all internal factory)
	 */
	void init();
	/**
	 * @brief Uninit the instance af algorithm (rm all internal factory)
	 */
	void init();
	/**
	 * @brief Create a process Algo with his name.
	 * @param[in] _name Name of the Algorithm.
	 * @return Instanciate algorithm 
	 */
	airtalgo::Algo* createAlgo(const std::string& _name);
	/**
	 * @brief Add a Factory Algorithm in the internal List (static for all instance)
	 * @param[in] _name Name of the Algorithm.
	 * @param[in] _functor Function of the factory
	 */
	void addAlgoFactory(const std::string& _name, std::function<airtalgo::Algo*(void)> _functor);
	/**
	 * @brief Remove a Factory Algorithm in the internal List (static for all instance)
	 * @param[in] _name Name of the Algorithm.
	 */
	void RmAlgoFactory(const std::string& _name);
};

#endif
