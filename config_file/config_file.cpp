#include "config_file.h"
#include "../strings_helper.h"

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

bool RZUF3_ConfigFile::getValue(std::string key, void* value)
{
	auto it = m_values.find(key);

	if (it == m_values.end())
	{
		spdlog::error("Config file {}: Key not found: {}", m_def->filepath, key);
		return false;
	}

	RZUF3_ConfigEntryDef* entry = it->second.first;
	std::string& strValue = it->second.second;

	return entry->parse(strValue, value);
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

	if (!entry->serialize(value, strValue))
	{
		spdlog::error("Config file {}: Could not serialize value for key: {}", m_def->filepath, key);
		return false;
	}

	if (m_def->autosave) save();

	return true;
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
		if (pos == std::string::npos || pos == 0 || pos == line.size() - 1) {
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
			continue;
		}
	}

	file.close();

	spdlog::info("Loaded config file: {}", filepath);
	m_loaded = true;
	return true;
}

bool RZUF3_ConfigFile::save()
{
	if(!m_loaded) return false;

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

	return true;
}
