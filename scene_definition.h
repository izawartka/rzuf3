#pragma once
#include "common.h"
#include "object_definition.h"
#include "asset_definition.h"

struct RZUF3_SceneDefinition {
public:
	std::string name;
	std::vector<RZUF3_AssetDefinition> staticAssets;
	std::vector<RZUF3_ObjectDefinition> objects;
};