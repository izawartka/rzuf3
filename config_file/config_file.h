#pragma once
#include "../common.h"
#include "config_file_def.h"
#include "entry_types/entry_types.h"

class RZUF3_ConfigFile {
public:
	RZUF3_ConfigFile(const RZUF3_ConfigFileDef& def);
	~RZUF3_ConfigFile();

	std::type_index getType(std::string key);
	bool getValue(std::string key, void* value);
	bool getValue(std::string key, void*& value, size_t& size);
	bool setValue(std::string key, void* value);

	bool isLoaded() const { return m_loaded; }
	std::string getFilepath() { return m_def->filepath; }

	bool saveConfig() { return save(); }

private:
	bool load();
	bool save();

	const RZUF3_ConfigFileDef* m_def;
	std::map<std::string, std::pair<RZUF3_ConfigEntryDef*, std::string>> m_values;
	bool m_loaded = false;
};