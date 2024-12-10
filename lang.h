#pragma once
#include "common.h"

class RZUF3_Lang {
public:
	RZUF3_Lang(std::string filepath);
	~RZUF3_Lang();

	bool load();
	std::string getText(std::string key);

	std::string m_filepath;
	std::unordered_map<std::string, std::string> m_texts;
	bool m_loaded = false;
};

extern RZUF3_Lang* g_lang;