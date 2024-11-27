#pragma once
#include "common.h"

struct RZUF3_TextStyle {
	std::string fontFilepath;
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Color bgColor = { 0, 0, 0, 0 };
	SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
	int size = 16;
	int style = TTF_STYLE_NORMAL;
};