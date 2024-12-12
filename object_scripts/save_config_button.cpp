#include "save_config_button.h"
#include "../game.h"

RZUF3_SaveConfigButton::RZUF3_SaveConfigButton(std::string configName)
{
	mp_options.configName = configName;
}

RZUF3_SaveConfigButton::RZUF3_SaveConfigButton(RZUF3_SaveConfigButtonOptions options)
{
	mp_options = options;
}

RZUF3_SaveConfigButton::~RZUF3_SaveConfigButton()
{

}

void RZUF3_SaveConfigButton::init()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, UIButtonClick);
}

void RZUF3_SaveConfigButton::deinit()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, UIButtonClick);
}

void RZUF3_SaveConfigButton::setConfigName(std::string configName)
{
	m_options.configName = configName;

	m_configFile = g_game->getConfigFile(configName);
}

void RZUF3_SaveConfigButton::saveConfig()
{
	if (m_configFile)
	{
		m_configFile->saveConfig();
	}
}

void RZUF3_SaveConfigButton::onUIButtonClick(RZUF3_UIButtonClickEvent* event)
{
	saveConfig();
}
