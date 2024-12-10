#pragma once
#include "../common.h"
#include "text_renderer_style.h"
#include "border_box_style.h"

enum class RZUF3_TextInputState {
	Normal,
	Focused,
};

#define RZUF3_TEXT_INPUT_STATE_COUNT 2

struct RZUF3_TextInputStyle {
	RZUF3_TextRendererStyle textStyle;
	SDL_Color textSelColor = { 0, 0, 0, 255 };
	SDL_Color textSelBgColor = { 255, 255, 255, 255 };
	RZUF3_BorderBoxStyle borderBoxStyle;
	SDL_Color blinkColor = { 255, 255, 255, 255 };
	SDL_Rect rect = { 0, 0, 128, 0 };
	int horizontalPadding = 5;
	int verticalPadding = 5;
	double blinkTime = 500.0;
};

struct RZUF3_TextInputStyleSet {
	RZUF3_TextInputStyle styles[RZUF3_TEXT_INPUT_STATE_COUNT];

	void copyFromNormal() {
		styles[(int)RZUF3_TextInputState::Focused] = styles[(int)RZUF3_TextInputState::Normal];
	}

	void defaultColors() {
		RZUF3_TextInputStyle& normal = styles[(int)RZUF3_TextInputState::Normal];
		normal.textStyle.defaultColors();
		normal.borderBoxStyle.color = { 128, 128, 128, 255 };
		normal.borderBoxStyle.infillColor = { 0, 0, 0, 255 };
		normal.blinkColor = { 128, 128, 128, 255 };
		normal.textSelColor = { 0, 0, 0, 255 };
		normal.textSelBgColor = { 128, 128, 128, 255 };

		RZUF3_TextInputStyle& focused = styles[(int)RZUF3_TextInputState::Focused];
		focused.textStyle.defaultColors();
		focused.borderBoxStyle.defaultColors();
		focused.blinkColor = { 255, 255, 255, 255 };
		focused.textSelColor = { 0, 0, 0, 255 };
		focused.textSelBgColor = { 255, 255, 255, 255 };
	}
};