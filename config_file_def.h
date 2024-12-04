#pragma once
#include "common.h"

struct RZUF3_ConfigFileDef {
	std::map<std::string, std::string> config;
	std::string filepath;
	bool isDefaultConfig = false;
	bool autosave = true;
};