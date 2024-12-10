#pragma once
#include "events.h"

class RZUF3_ConfigFile;

class RZUF3_ConfigSaveLoadEvent : public RZUF3_Event {
public:
	RZUF3_ConfigSaveLoadEvent(RZUF3_ConfigFile* configFile) {
		m_configFile = configFile;
	}

	RZUF3_ConfigFile* getConfigFile() const { return m_configFile; }

private:
	RZUF3_ConfigFile* m_configFile;
};