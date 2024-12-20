#include "../events/events.h"
#include "../strings_helper.h"
#include "../scene.h"
#include "config_file.h"

RZUF3_ConfigFile::RZUF3_ConfigFile(const RZUF3_ConfigFileDef& def)
{
	m_def = &def;

	for (auto& entry : def.entries)
	{
		RZUF3_ConfigEntryDef* entryDef = entry.second.get();
		RZUF3_ConfigFileEntry newEntry = { entryDef, entryDef->getDefaultValue() };
		m_entries.insert(std::make_pair(entry.first, newEntry));
	}

	if (!load()) {
		spdlog::warn("Config file {}: Could not load file, using default values", def.filepath);
		save();
	}
}

RZUF3_ConfigFile::~RZUF3_ConfigFile()
{

}

bool RZUF3_ConfigFile::setValueRaw(std::string key, void* value, std::type_index type)
{
	if(value == nullptr) return false;

	RZUF3_ConfigFileEntry* entry = getEntryRaw(key);
	if (entry == nullptr) return false;

	if (type != entry->def->getType())
	{
		spdlog::error("Config file {}: Type mismatch while setting key: {}", m_def->filepath, key);
		return false;
	}

	std::string& strValue = entry->stringValue;
	std::string newStrValue;

	if (!entry->def->serialize(value, newStrValue))
	{
		spdlog::error("Config file {}: Could not serialize value for key: {}", m_def->filepath, key);
		return false;
	}

	if(m_def->ignoreUnchanged && strValue == newStrValue) return true;
	strValue = newStrValue;

	bool saved = false;
	if (m_def->autosave) saved = save();

	for (auto& listener : m_specialListeners[key]) {
		listener.second(saved);
	}

	RZUF3_ConfigEntryUpdateEvent updateEvent(this, key, saved);
	if (g_scene) g_scene->getEventsManager()->dispatchEvent(&updateEvent);

	return true;
}

int RZUF3_ConfigFile::addSpecialListener(std::string key, RZUF3_ConfigFileSpecialListener listener)
{
	RZUF3_ConfigEntryDef* entry = getEntryRaw(key)->def;
	if (entry == nullptr) {
		spdlog::error("Config file {}: Cannot add special listener", m_def->filepath);
		return -1;
	}

	m_specialListeners[key][m_nextSpecialListenerId] = listener;

	return m_nextSpecialListenerId++;
}

bool RZUF3_ConfigFile::removeSpecialListener(int id)
{
	for (auto& entry : m_specialListeners)
	{
		auto it = entry.second.find(id);
		if (it != entry.second.end())
		{
			entry.second.erase(it);
			return true;
		}
	}

	spdlog::error("Config file {}: Cannot remove special listener, id not found: {}", m_def->filepath, id);

	return false;
}

RZUF3_ConfigFileEntry* RZUF3_ConfigFile::getEntryRaw(std::string key)
{
	auto it = m_entries.find(key);

	if (it == m_entries.end())
	{
		spdlog::error("Config file {}: Key not found: {}", m_def->filepath, key);
		return nullptr;
	}

	return &it->second;
}

bool RZUF3_ConfigFile::load()
{
	if (!m_def) {
		spdlog::error("Config file: Definition is null");
		return false;
	}

	const std::string& filepath = m_def->filepath;
	std::ifstream file(filepath, std::ios::in);

	if (!file.is_open())
	{
		spdlog::warn("Config file {}: Could not open file for reading", filepath);
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if(line.empty() || line[0] == '#')
			continue;

		size_t pos = line.find('=');
		if (pos == std::string::npos || pos == 0) {
			spdlog::warn("Config file {}: Invalid line: {}", filepath, line);
			continue;
		}

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		RZUF3_StringsHelper::trim(key);
		RZUF3_StringsHelper::trim(value);

		auto it = m_entries.find(key);
		if (it == m_entries.end()) continue;

		RZUF3_ConfigFileEntry& entry = it->second;
		if (!entry.def->validate(value)) {
			spdlog::warn("Config file {}: Invalid value for key {}, using default value", filepath, key);
			value = entry.def->getDefaultValue();
		}

		entry.stringValue = value;

		for (auto& listener : m_specialListeners[key]) {
			listener.second(true);
		}

		if (g_scene) {
			RZUF3_ConfigEntryUpdateEvent updateEvent(this, key, true);
			g_scene->getEventsManager()->dispatchEvent(&updateEvent);
		}
	}

	file.close();

	spdlog::info("Loaded config file: {}", filepath);

	return true;
}

bool RZUF3_ConfigFile::save()
{
	const std::string& filepath = m_def->filepath;
	std::ofstream file(filepath, std::ios::out | std::ios::trunc);

	if (!file.is_open())
	{
		spdlog::error("Config file {}: Could not open file for writing", filepath);
		return false;
	}

	for (auto it = m_entries.begin(); it != m_entries.end(); it++)
	{
		file << it->first << "=" << it->second.stringValue << std::endl;
	}

	file.close();

	RZUF3_ConfigSaveEvent saveEvent(this);
	if(g_scene) g_scene->getEventsManager()->dispatchEvent(&saveEvent);

	return true;
}
