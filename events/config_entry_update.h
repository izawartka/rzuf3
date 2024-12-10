#pragma once
#include "events.h"

class RZUF3_ConfigFile;

class RZUF3_ConfigEntryUpdateEvent : public RZUF3_Event {
public:
	RZUF3_ConfigEntryUpdateEvent(RZUF3_ConfigFile* configFile, std::string key, bool isSaved) {
		m_configFile = configFile;
		m_key = key;
		m_isSaved = isSaved;
	}

	RZUF3_ConfigFile* getConfigFile() const { return m_configFile; }
	std::string getKey() const { return m_key; }
	bool isSaved() const { return m_isSaved; }

private:
	RZUF3_ConfigFile* m_configFile;
	std::string m_key;
	bool m_isSaved;
};