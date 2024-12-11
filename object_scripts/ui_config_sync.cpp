#include "ui_config_sync.h"
#include "../game.h"

RZUF3_UIConfigSync::RZUF3_UIConfigSync(std::string configName, std::string key)
{
	mp_options.configName = configName;
	mp_options.key = key;
}

RZUF3_UIConfigSync::RZUF3_UIConfigSync(RZUF3_UIConfigSyncOptions options)
{
	mp_options = m_options;
}

RZUF3_UIConfigSync::~RZUF3_UIConfigSync()
{

}

void RZUF3_UIConfigSync::init()
{
	m_options = mp_options;

	setConfigName(m_options.configName);
	setUseOnConfigEntryUpdate(m_options.useOnConfigEntryUpdate);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, UIValueChange);

	if(m_options.updateUIValueOnInit) updateUIValue();
}

void RZUF3_UIConfigSync::deinit()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, UIValueChange);

	setUseOnConfigEntryUpdate(false);

	m_configFile = nullptr;
	m_type = typeid(void);
}

void RZUF3_UIConfigSync::setConfigName(std::string configName)
{
	m_options.configName = configName;
	m_configFile = g_game->getConfigFile(configName);
	updateType();
}

void RZUF3_UIConfigSync::setKey(std::string key)
{
	m_options.key = key;
	updateType();
}

void RZUF3_UIConfigSync::setSavedChangesOnly(bool savedChangesOnly)
{
	m_options.savedChangesOnly = savedChangesOnly;
}

void RZUF3_UIConfigSync::setUseOnConfigEntryUpdate(bool useOnConfigEntryUpdate)
{
	m_options.useOnConfigEntryUpdate = useOnConfigEntryUpdate;
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();

	if (useOnConfigEntryUpdate && !m_hasOnConfigEntryUpdateListener) {
		_ADD_LISTENER(eventsManager, ConfigEntryUpdate);
		m_hasOnConfigEntryUpdateListener = true;
	}
	else if (!useOnConfigEntryUpdate && m_hasOnConfigEntryUpdateListener) {
		_REMOVE_LISTENER(eventsManager, ConfigEntryUpdate);
		m_hasOnConfigEntryUpdateListener = false;
	}
}

void RZUF3_UIConfigSync::updateUIValue()
{
	if (!m_configFile) return;
	if (m_type == typeid(void)) return;

	void* value = nullptr;
	size_t size = 0;

	if (!m_configFile->getValue(m_options.key, value, size)) return;

	m_tempDisableOnUIValueChange = true;

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	RZUF3_UISetValueEvent objEvent(m_type, value, size);
	objEventsManager->dispatchEvent(&objEvent);

	m_tempDisableOnUIValueChange = false;
	delete value;
}

void RZUF3_UIConfigSync::onConfigEntryUpdate(RZUF3_ConfigEntryUpdateEvent* event)
{
	if(!m_options.useOnConfigEntryUpdate) return;
	if(m_configFile != event->getConfigFile()) return;
	if (m_options.key != event->getKey()) return;
	if(m_options.savedChangesOnly && !event->isSaved()) return;

	updateUIValue();
}

void RZUF3_UIConfigSync::onUIValueChange(RZUF3_UIValueChangeEvent* event)
{
	if (m_tempDisableOnUIValueChange) return;
	if (m_configFile == nullptr) return;
	if (m_type == typeid(void)) return;
	if (m_type != event->getTypeIndex()) return;

	void* value = event->getValue();

	m_configFile->setValue(m_options.key, value);
}

void RZUF3_UIConfigSync::updateType()
{
	m_type = typeid(void);

	if (m_configFile == nullptr) return;
	m_type = m_configFile->getType(m_options.key);
}