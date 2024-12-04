#pragma once
#include "common.h"
#include "config_file_def.h"

class RZUF3_ConfigFile {
public:
	RZUF3_ConfigFile(RZUF3_ConfigFileDef def);
	~RZUF3_ConfigFile();

	std::string getValue(std::string key);
	void setValue(std::string key, std::string value);

	bool isLoaded() const { return m_loaded; }
	std::string getFilepath() { return m_filepath; }

	bool saveConfig() { return save(); }

private:
	bool load();
	bool save();

	std::string m_filepath;
	std::unordered_map<std::string, std::string> m_values;
	bool m_loaded = false;
	bool m_autosave = false;
};