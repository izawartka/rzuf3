#pragma once
#include "common.h"
#include "asset.h"

class RZUF3_Asset;

struct RZUF3_AssetDefinition
{
	std::string filepath;
	std::function<RZUF3_Asset*(std::string)> factory;
};