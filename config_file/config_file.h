#pragma once
#include "../common.h"
#include "config_file_def.h"
#include "entry_types/entry_types.h"

typedef std::function<void(bool isSaved)> RZUF3_ConfigFileSpecialListener;

class RZUF3_ConfigFile {
public:
	RZUF3_ConfigFile(const RZUF3_ConfigFileDef& def);
	~RZUF3_ConfigFile();

	bool setValue(std::string key, void* value);
	bool saveConfig() { return save(); }
	int addSpecialListener(std::string key, RZUF3_ConfigFileSpecialListener listener);
	bool removeSpecialListener(int id);

	std::type_index getType(std::string key);
	bool getValue(std::string key, void* value);
	bool getValue(std::string key, void*& value, size_t& size);

	std::string getFilepath() { return m_def->filepath; }
private:
	bool load();
	bool save();

	const RZUF3_ConfigFileDef* m_def;
	std::map<std::string, std::pair<RZUF3_ConfigEntryDef*, std::string>> m_values;
	std::map<std::string, std::map<int, RZUF3_ConfigFileSpecialListener>> m_specialListeners;
	int m_nextSpecialListenerId = 0;
};