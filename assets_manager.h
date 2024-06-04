#pragma once
#include "common.h"
#include "asset.h"
#include "asset_definition.h"
#include "renderer.h"

class RZUF3_Asset;
struct RZUF3_AssetDefinition;
class RZUF3_Renderer;

class RZUF3_AssetsManager {
public:
	RZUF3_AssetsManager(RZUF3_Renderer* renderer);
	~RZUF3_AssetsManager();
	
	bool addAsset(RZUF3_AssetDefinition assetDefinition);
	void addAssets(std::vector<RZUF3_AssetDefinition> assetDefinitions);
	bool removeAsset(std::string filepath);
	void removeAssets(std::vector<std::string> filepaths);

	RZUF3_Asset* getAsset(std::string filepath);
	void* getAssetContent(std::string filepath);

	RZUF3_Renderer* getRenderer() { return m_renderer; }

private:
	RZUF3_Renderer* m_renderer = nullptr;
	std::map<std::string, RZUF3_Asset*> m_assets = std::map<std::string, RZUF3_Asset*>();
};