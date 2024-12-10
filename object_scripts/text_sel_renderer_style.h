#pragma once
#include "../common.h"
#include "text_renderer_style.h"

struct RZUF3_TextSelStyle {
	RZUF3_TextRendererStyle textStyle;
	SDL_Color textSelColor = { 0, 0, 0, 255 };
	SDL_Color textSelBgColor = { 255, 255, 255, 255 };

	void defaultColors() {
		textStyle.defaultColors();
		textSelColor = { 0, 0, 0, 255 };
		textSelBgColor = { 255, 255, 255, 255 };
	}
};