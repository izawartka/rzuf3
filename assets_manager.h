#pragma once
#include "common.h"
#include "asset.h"
#include "asset_definition.h"

class RZUF3_Asset;
struct RZUF3_AssetDefinition;

class RZUF3_AssetsManager {
public:
	RZUF3_AssetsManager();
	~RZUF3_AssetsManager();
	
	RZUF3_Asset* addAsset(RZUF3_AssetDefinition assetDefinition);
	std::vector<RZUF3_Asset*> addAssets(std::vector<RZUF3_AssetDefinition> assetDefinitions);

	bool removeAsset(std::string filepath);
	void removeAssets(std::vector<std::string> filepaths);

	RZUF3_Asset* getAsset(std::string filepath);
	void* getAssetContent(std::string filepath);

private:
	std::map<std::string, RZUF3_Asset*> m_assets = std::map<std::string, RZUF3_Asset*>();
};