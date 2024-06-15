#include "lang.h"
#include "strings_helper.h"

RZUF3_Lang* g_lang = nullptr;

RZUF3_Lang::RZUF3_Lang(std::string filepath)
{
	if(g_lang != nullptr) throw std::logic_error("Only one instance of RZUF3_Lang can be present at a time");
	g_lang = this;

	m_filepath = filepath;
}

RZUF3_Lang::~RZUF3_Lang()
{
	spdlog::info("Unloaded language file: {}", m_filepath);

	g_lang = nullptr;
}

bool RZUF3_Lang::load()
{
	m_loaded = false;

	std::ifstream file(m_filepath);
	if (!file.is_open()) {
		spdlog::error("Failed to open the language file: {}", m_filepath);
		return false;
	}

	m_texts.clear();
	std::string line;
	std::string accumulatedLine;

	while (std::getline(file, line)) {
		if (line.empty() || line.at(0) == '#') continue;

		accumulatedLine += line;

		if (accumulatedLine.back() == '\\') {
			accumulatedLine.pop_back();
			continue;
		}

		std::string key, value;
		size_t pos = accumulatedLine.find('=');

		if (pos == std::string::npos) {
			spdlog::error("Language file: Invalid line: {}", accumulatedLine);
			accumulatedLine = "";
			continue;
		}

		key = accumulatedLine.substr(0, pos);
		value = accumulatedLine.substr(pos + 1);

		RZUF3_StringsHelper::trim(key);
		RZUF3_StringsHelper::trim(value);

		if (m_texts.find(key) != m_texts.end()) {
			spdlog::error("Language file: Duplicate key: {}", key);
			accumulatedLine = "";
			continue;
		}

		m_texts.insert(std::pair(key, value));
		accumulatedLine = "";
	}

	m_loaded = true;
	spdlog::info("Loaded language file: {} ({} entries)", m_filepath, m_texts.size());

	return true;
}

std::string RZUF3_Lang::getText(std::string key)
{
	if (!m_loaded) return key;

	auto it = m_texts.find(key);

	if (it == m_texts.end()) {
		spdlog::warn("Language file: No entry with a key {} found in the file {}", key, m_filepath);
		return key;
	}

	return it->second;
}
