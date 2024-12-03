#pragma once
#include "../common.h"
#include "../asset.h"

class RZUF3_Font : public RZUF3_Asset {
public:
	static RZUF3_Font* getInstance(std::string filepath);

	bool load();
	void unload();
	void* getContent();

private:
	TTF_Font* m_font = nullptr;
};