/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <ewol/ewol.h>
#include <appl/debug.h>
#include <appl/Windows.h>
#include <ewol/widget/Label.h>
#include <ewol/widget/Entry.h>

#undef __class__
#define __class__ "Windows"

appl::Windows::Windows() {
	addObjectType("appl::Windows");
}

void appl::Windows::init() {
	ewol::widget::Windows::init();
	setTitle("Drain Equalizer Profiler");
	
	m_gui = ewol::widget::Composer::create(ewol::widget::Composer::file, "DATA:gui.xml");
	if (m_gui != nullptr) {
		setSubWidget(m_gui);
	}
	
	subBind(ewol::widget::Entry, "sample-rate", signalModify, shared_from_this(), &appl::Windows::onCallbackFrequency);
	
	
}

void appl::Windows::onCallbackFrequency(const std::string& _value) {
	APPL_INFO("plop " << _value);
}

