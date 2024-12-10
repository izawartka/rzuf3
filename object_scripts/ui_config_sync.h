#pragma once
#include "../events/events.h"
#include "../event_macros.h"
#include "../config_file/config_file.h"
#include "object_scripts.h"

struct RZUF3_UIConfigSyncOptions {
	std::string configName;
	std::string key;
	bool savedChangesOnly = false;
	bool updateUIValueOnInit = true;
	bool useOnConfigEntryUpdate = true;
};

class RZUF3_UIConfigSync : public RZUF3_ObjectScript {
public:
	RZUF3_UIConfigSync(std::string configName, std::string key);
	RZUF3_UIConfigSync(RZUF3_UIConfigSyncOptions options);
	~RZUF3_UIConfigSync();

	void init();
	void deinit();

	void setConfigName(std::string configName);
	void setKey(std::string key);
	void setSavedChangesOnly(bool savedChangesOnly);
	void setUseOnConfigEntryUpdate(bool useOnConfigEntryUpdate);

	void updateUIValue();

	std::string getConfigName() const { return m_options.configName; }
	std::string getKey() const { return m_options.key; }
	std::type_index getType() const { return m_type; }
	bool getSavedChangesOnly() const { return m_options.savedChangesOnly; }
	bool getUseOnConfigEntryUpdate() const { return m_options.useOnConfigEntryUpdate; }

protected:
	void onConfigEntryUpdate(RZUF3_ConfigEntryUpdateEvent* event);
	void onUIValueChange(RZUF3_UIValueChangeEvent* event);

	void updateType();

	RZUF3_UIConfigSyncOptions mp_options;

	RZUF3_UIConfigSyncOptions m_options;
	RZUF3_ConfigFile* m_configFile = nullptr;
	std::type_index m_type = typeid(void);
	bool m_hasOnConfigEntryUpdateListener = false;
	bool m_tempDisableOnUIValueChange = false;

	_DECLARE_LISTENER(ConfigEntryUpdate)
	_DECLARE_LISTENER(UIValueChange)
};