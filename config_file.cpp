#include "config_file.h"
#include "strings_helper.h"

RZUF3_ConfigFile::RZUF3_ConfigFile(RZUF3_ConfigFileDef def)
{
	for (auto& entry : def.config)
	{
		m_values[entry.first] = entry.second;
	}

	m_filepath = def.filepath;
	m_autosave = def.autosave;

	if (!load()) {
		spdlog::info("Creating new config file {}", m_filepath);
		save();
	}
}

RZUF3_ConfigFile::~RZUF3_ConfigFile()
{

}

std::string RZUF3_ConfigFile::getValue(std::string key)
{
	auto it = m_values.find(key);

	if (it != m_values.end())
	{
		return it->second;
	}
	else
	{
		spdlog::error("Config file {}: Key not found: {}", m_filepath, key);
		return "";
	}
}

void RZUF3_ConfigFile::setValue(std::string key, std::string value)
{
	auto it = m_values.find(key);

	if (it != m_values.end())
	{
		it->second = value;

		if(m_autosave) save();
	}
	else
	{
		spdlog::error("Config file {}: Key not found: {}", m_filepath, key);
	}
}

bool RZUF3_ConfigFile::load()
{
	std::ifstream file(m_filepath, std::ios::in);

	if (!file.is_open())
	{
		spdlog::warn("Config file {}: Could not open file for reading", m_filepath);
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if(line.empty() || line[0] == '#')
			continue;

		std::string key;
		std::string value;

		size_t pos = line.find('=');
		if (pos != std::string::npos)
		{
			key = line.substr(0, pos);
			value = line.substr(pos + 1);

			RZUF3_StringsHelper::trim(key);
			RZUF3_StringsHelper::trim(value);

			auto it = m_values.find(key);
			if (it != m_values.end()) it->second = value;
		}
	}

	file.close();

	spdlog::info("Loaded config file: {}", m_filepath);
	return true;
}

bool RZUF3_ConfigFile::save()
{
	std::ofstream file(m_filepath, std::ios::out | std::ios::trunc);

	if (!file.is_open())
	{
		spdlog::error("Config file {}: Could not open file for writing", m_filepath);
		return false;
	}

	for (auto it = m_values.begin(); it != m_values.end(); it++)
	{
		file << it->first << "=" << it->second << std::endl;
	}

	file.close();

	return true;
}
