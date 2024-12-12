#pragma once
#include "../events/events.h"
#include "../event_macros.h"
#include "../config_file/config_file.h"
#include "object_scripts.h"

struct RZUF3_SaveConfigButtonOptions {
	std::string configName;
};

class RZUF3_SaveConfigButton : public RZUF3_ObjectScript {
public:
	RZUF3_SaveConfigButton(std::string configName);
	RZUF3_SaveConfigButton(RZUF3_SaveConfigButtonOptions options);
	~RZUF3_SaveConfigButton();

	void init();
	void deinit();

	void setConfigName(std::string configName);

	void saveConfig();

	std::string getConfigName() const { return m_options.configName; }

protected:
	void onUIButtonClick(RZUF3_UIButtonClickEvent* event);

	RZUF3_SaveConfigButtonOptions mp_options;

	RZUF3_SaveConfigButtonOptions m_options;
	RZUF3_ConfigFile* m_configFile = nullptr;

	_DECLARE_LISTENER(UIButtonClick)
};