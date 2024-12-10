#pragma once
#include "../common.h"
#include "border_box_style.h"
#include "text_renderer_style.h"

enum class RZUF3_TextButtonState {
	Normal,
	NormalHighlighted,
	Hover,
	Pressed
};

#define RZUF3_TEXT_BUTTON_STATE_COUNT 4

struct RZUF3_TextButtonStyle {
	RZUF3_BorderBoxStyle borderBoxStyle;
	RZUF3_TextRendererStyle textStyle;
	int horizontalPadding = 5;
	int verticalPadding = 5;
	SDL_Rect rect = { 0, 0, 0, 0 };
};

struct RZUF3_TextButtonStyleSet {
	RZUF3_TextButtonStyle styles[RZUF3_TEXT_BUTTON_STATE_COUNT];

	void defaultColors() {
		RZUF3_TextButtonStyle& normalStyle = styles[(int)RZUF3_TextButtonState::Normal];
		normalStyle.borderBoxStyle.color = { 128, 128, 128, 255 };
		normalStyle.borderBoxStyle.infillColor = { 0, 0, 0, 255 };
		normalStyle.textStyle.color = { 255, 255, 255, 255 };
		normalStyle.textStyle.bgColor = { 0, 0, 0, 255 };

		RZUF3_TextButtonStyle& normalHighlightedStyle = styles[(int)RZUF3_TextButtonState::NormalHighlighted];
		normalHighlightedStyle.borderBoxStyle.color = { 128, 128, 255, 255 };
		normalHighlightedStyle.borderBoxStyle.infillColor = { 32, 32, 64, 255 };
		normalHighlightedStyle.textStyle.color = { 255, 255, 255, 255 };
		normalHighlightedStyle.textStyle.bgColor = { 32, 32, 64, 255 };

		RZUF3_TextButtonStyle& hoverStyle = styles[(int)RZUF3_TextButtonState::Hover];
		hoverStyle.borderBoxStyle.color = { 64, 64, 128, 255 };
		hoverStyle.borderBoxStyle.infillColor = { 0, 0, 0, 255 };
		hoverStyle.textStyle.color = { 255, 255, 255, 255 };
		hoverStyle.textStyle.bgColor = { 0, 0, 0, 255 };

		RZUF3_TextButtonStyle& pressedStyle = styles[(int)RZUF3_TextButtonState::Pressed];
		pressedStyle.borderBoxStyle.color = { 64, 64, 128, 255 };
		pressedStyle.borderBoxStyle.infillColor = { 32, 32, 64, 255 };
		pressedStyle.textStyle.color = { 255, 255, 255, 255 };
		pressedStyle.textStyle.bgColor = { 32, 32, 64, 255 };
	}

	void copyFromNormal() {
		styles[(int)RZUF3_TextButtonState::NormalHighlighted] = styles[(int)RZUF3_TextButtonState::Normal];
		styles[(int)RZUF3_TextButtonState::Hover] = styles[(int)RZUF3_TextButtonState::Normal];
		styles[(int)RZUF3_TextButtonState::Pressed] = styles[(int)RZUF3_TextButtonState::Normal];
	}
};