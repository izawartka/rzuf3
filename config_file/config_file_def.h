#pragma once
#include "../common.h"
#include "config_entry_def.h"
#include "entry_types/entry_types.h"

struct RZUF3_ConfigFileDef {
    std::map<std::string, std::unique_ptr<RZUF3_ConfigEntryDef>> entries;
    std::string filepath;
    bool isDefaultConfig = false;
    bool autosave = true;
    bool ignoreUnchanged = true;

    RZUF3_ConfigFileDef(std::initializer_list<std::pair<std::string, std::unique_ptr<RZUF3_ConfigEntryDef>>> configEntries,
        const std::string& filepath, bool isDefaultConfig, bool autosave, bool ignoreUnchanged)
        : filepath(filepath), isDefaultConfig(isDefaultConfig), autosave(autosave), ignoreUnchanged(ignoreUnchanged) {

        for (auto& entry : configEntries) {
            entries.insert(std::make_pair(entry.first, entry.second->clone()));
		}
    }
};