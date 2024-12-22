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

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);

	m_willUpdateUIValue = m_options.updateUIValueOnInit;
}

void RZUF3_UIConfigSync::deinit()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, UIValueChange);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Update);

	setUseOnConfigEntryUpdate(false);

	m_tempDisableOnUIValueChange = false;
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
		m_tempDisableOnConfigEntryUpdate = false;
	}
}

void RZUF3_UIConfigSync::updateUIValue()
{
	m_willUpdateUIValue = false;
	if (!m_configFile) return;
	if (m_type == typeid(void)) return;

	RZUF3_ConfigFileEntry* entry = m_configFile->getEntryRaw(m_options.key);
	if (entry == nullptr) return;

	size_t size = 0;
	void* value = nullptr;

	if (!entry->def->parse(entry->stringValue, value, size)) return;

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	RZUF3_UISetValueEvent objEvent(m_type, value, size);

	m_tempDisableOnUIValueChange = true;
	objEventsManager->dispatchEvent(&objEvent);
	m_tempDisableOnUIValueChange = false;

	entry->def->destroyValue(value);
}

void RZUF3_UIConfigSync::onConfigEntryUpdate(RZUF3_ConfigEntryUpdateEvent* event)
{
	if(m_tempDisableOnConfigEntryUpdate) return;
	if(!m_options.useOnConfigEntryUpdate) return;
	if(m_configFile != event->getConfigFile()) return;
	if (m_options.key != event->getKey()) return;
	if(m_options.savedChangesOnly && !event->isSaved()) return;

	updateUIValue();
}

void RZUF3_UIConfigSync::onUpdate(RZUF3_UpdateEvent* event)
{
	if (m_willUpdateUIValue) updateUIValue();
}

void RZUF3_UIConfigSync::onUIValueChange(RZUF3_UIValueChangeEvent* event)
{
	if (m_tempDisableOnUIValueChange) return;
	if (m_configFile == nullptr) return;
	if (m_type == typeid(void)) return;
	if (m_type != event->getTypeIndex()) return;

	void* value = event->getValue();

	m_tempDisableOnConfigEntryUpdate = true;
	if (!m_configFile->setValueRaw(m_options.key, value, m_type) && m_options.revertIfSetConfigValueFailed) {
		updateUIValue();
	}
	m_tempDisableOnConfigEntryUpdate = false;
}

void RZUF3_UIConfigSync::updateType()
{
	m_type = typeid(void);

	if (m_configFile == nullptr) return;

	RZUF3_ConfigFileEntry* entry = m_configFile->getEntryRaw(m_options.key);
	if (entry == nullptr) return;

	m_type = entry->def->getType();
}