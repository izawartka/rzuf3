#pragma once
#include "../common.h"

struct RZUF3_BorderBoxStyle {
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Color infillColor = { 0, 0, 0, 0 };
	int width = 2;

	void defaultColors() {
		color = { 255, 255, 255, 255 };
		infillColor = { 0, 0, 0, 0 };
	}
};