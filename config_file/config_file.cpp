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
		auto pair = std::make_pair(entryDef, entryDef->getDefaultValue());
		m_values.insert(std::make_pair(entry.first, pair));
	}

	if (!load()) {
		spdlog::warn("Config file {}: Could not load file, using default values", def.filepath);
		save();
	}
}

RZUF3_ConfigFile::~RZUF3_ConfigFile()
{

}


bool RZUF3_ConfigFile::setValue(std::string key, void* value)
{
	auto it = m_values.find(key);

	if (it == m_values.end())
	{
		spdlog::error("Config file {}: Key not found: {}", m_def->filepath, key);
		return false;
	}

	RZUF3_ConfigEntryDef* entry = it->second.first;
	std::string& strValue = it->second.second;
	std::string newStrValue;

	if (!entry->serialize(value, newStrValue))
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
	auto it = m_values.find(key);

	if (it == m_values.end())
	{
		spdlog::error("Config file {}: Cannot add special listener, key not found: {}", m_def->filepath, key);
		return -1;
	}

	RZUF3_ConfigEntryDef* entry = it->second.first;

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

std::type_index RZUF3_ConfigFile::getType(std::string key)
{
	auto it = m_values.find(key);

	if (it == m_values.end())
	{
		spdlog::error("Config file {}: Key not found: {}", m_def->filepath, key);
		return typeid(void);
	}

	return it->second.first->getType();
}

bool RZUF3_ConfigFile::getValue(std::string key, void* value)
{
	void* valuePtr = nullptr;
	size_t size = 0;

	if (!getValue(key, valuePtr, size)) return false;

	memcpy(value, valuePtr, size);
	delete valuePtr;

	return true;
}

bool RZUF3_ConfigFile::getValue(std::string key, void*& value, size_t& size)
{
	auto it = m_values.find(key);

	if (it == m_values.end())
	{
		spdlog::error("Config file {}: Key not found: {}", m_def->filepath, key);
		return false;
	}

	RZUF3_ConfigEntryDef* entry = it->second.first;
	std::string& strValue = it->second.second;

	return entry->parse(strValue, value, size);
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

		auto it = m_values.find(key);
		if (it == m_values.end()) continue;

		RZUF3_ConfigEntryDef* entry = it->second.first;
		if (!entry->validate(value)) {
			spdlog::warn("Config file {}: Invalid value for key {}, using default value", filepath, key);
			value = entry->getDefaultValue();
		}

		it->second.second = value;

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

	for (auto it = m_values.begin(); it != m_values.end(); it++)
	{
		file << it->first << "=" << it->second.second << std::endl;
	}

	file.close();

	RZUF3_ConfigSaveEvent saveEvent(this);
	if(g_scene) g_scene->getEventsManager()->dispatchEvent(&saveEvent);

	return true;
}
