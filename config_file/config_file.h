#pragma once
#include "../common.h"
#include "config_file_def.h"
#include "entry_types/entry_types.h"

typedef std::function<void(bool isSaved)> RZUF3_ConfigFileSpecialListener;

struct RZUF3_ConfigFileEntry {
	RZUF3_ConfigEntryDef* def;
	std::string stringValue;
};

class RZUF3_ConfigFile {
public:
	RZUF3_ConfigFile(const RZUF3_ConfigFileDef& def);
	~RZUF3_ConfigFile();

	template<class T>
	bool setValue(std::string key, T value)
	{
		return setValueRaw(key, (void*)&value, std::type_index(typeid(T)));
	}

	template<class T>
	bool getValue(std::string key, T& value)
	{
		RZUF3_ConfigFileEntry* entry = getEntryRaw(key);
		if (entry == nullptr) return false;
		
		if (std::type_index(typeid(T)) != entry->def->getType())
		{
			spdlog::error("Config file {}: Type mismatch while getting key: {}", m_def->filepath, key);
			return false;
		}

		size_t size = 0;
		T* valuePtr = nullptr;
		if(!entry->def->parse(entry->stringValue, (void*&)valuePtr, size)) return false;
		if(valuePtr == nullptr || size == 0) return false;

		value = *valuePtr;
		entry->def->destroyValue(valuePtr);

		return true;
	}

	bool setValueRaw(std::string key, void* value, std::type_index type);
	bool saveConfig() { return save(); }
	int addSpecialListener(std::string key, RZUF3_ConfigFileSpecialListener listener);
	bool removeSpecialListener(int id);

	RZUF3_ConfigFileEntry* getEntryRaw(std::string key);

	std::string getFilepath() { return m_def->filepath; }
private:
	bool load();
	bool save();

	const RZUF3_ConfigFileDef* m_def;
	std::map<std::string, RZUF3_ConfigFileEntry> m_entries;
	std::map<std::string, std::map<int, RZUF3_ConfigFileSpecialListener>> m_specialListeners;
	int m_nextSpecialListenerId = 0;
};