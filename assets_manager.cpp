#include "assets_manager.h"

RZUF3_AssetsManager::RZUF3_AssetsManager()
{

}

RZUF3_AssetsManager::~RZUF3_AssetsManager()
{
	for (auto& asset : m_assets)
	{
		spdlog::debug(
			"Asset {} was not unloaded by its owner. (ignore this message if the asset is static)",
			asset.first
		);
		asset.second->unload();
		delete asset.second;
	}
}

RZUF3_Asset* RZUF3_AssetsManager::addAsset(RZUF3_AssetDefinition assetDefinition)
{
	if(assetDefinition.filepath.empty())
	{
		spdlog::error("Filepath of the asset to add is empty");
		return nullptr;
	}

	auto it = m_assets.find(assetDefinition.filepath);

	if (it != m_assets.end())
	{
		spdlog::debug("Reusing asset {}", it->second->getFilepath());
		it->second->addReference();
		return it->second;
	}

	RZUF3_Asset* asset = assetDefinition.factory(assetDefinition.filepath);

	std::string filepath = asset->getFilepath();

	if (!asset->load())
	{
		return nullptr;
	}

	asset->addReference();

	m_assets[filepath] = asset;
	return asset;
}

std::vector<RZUF3_Asset*> RZUF3_AssetsManager::addAssets(std::vector<RZUF3_AssetDefinition> assetDefinitions)
{
	std::vector<RZUF3_Asset*> assets;

	for(RZUF3_AssetDefinition assetDefinition : assetDefinitions)
	{
		RZUF3_Asset* asset = addAsset(assetDefinition);
		assets.push_back(asset);
	}

	return assets;
}

bool RZUF3_AssetsManager::removeAsset(std::string filename)
{
	if(filename.empty())
	{
		spdlog::error("Filepath of the asset to remove is empty");
		return false;
	}

	auto it = m_assets.find(filename);

	if (it == m_assets.end())
	{
		spdlog::warn("Asset {} is not loaded", filename);
		return false;
	}

	RZUF3_Asset* asset = it->second;

	asset->removeReference();

	if(asset->getReferences() > 0)
	{
		return true;
	}

	m_assets.erase(filename);
	asset->unload();
	delete asset;

	return true;
}

void RZUF3_AssetsManager::removeAssets(std::vector<std::string> filepaths)
{
	for (std::string filepath : filepaths)
	{
		removeAsset(filepath);
	}
}

RZUF3_Asset* RZUF3_AssetsManager::getAsset(std::string filepath)
{
	auto it = m_assets.find(filepath);

	if (it == m_assets.end())
	{
		spdlog::error("Asset {} is not loaded", filepath);
		return nullptr;
	}

	return it->second;
}

void* RZUF3_AssetsManager::getAssetContent(std::string filepath)
{
	return getAsset(filepath)->getContent();
}