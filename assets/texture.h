#pragma once
#include "../common.h"
#include "../asset.h"

class RZUF3_Texture : public RZUF3_Asset {
public:
	static RZUF3_Texture* getInstance(std::string filepath);

	bool load(RZUF3_AssetsManager* assetsManager);
	void unload();
	void* getContent();

private:
	SDL_Texture* m_texture = nullptr;
};