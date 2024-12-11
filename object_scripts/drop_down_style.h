#pragma once
#include "../common.h"
#include "text_button_style.h"
#include "border_box_style.h"

struct RZUF3_DropDownStyle {
	RZUF3_TextButtonStyleSet button;
	RZUF3_BorderBoxStyle itemsBorder;
	RZUF3_TextButtonStyleSet item;
	int arrowSize = 4;
	int arrowOffset = 4;

	void defaultColors() {
		button.defaultColors();
		item.defaultColors();

		itemsBorder.color = { 128, 128, 128, 255 };
		itemsBorder.infillColor = { 0, 0, 0, 255 };

		RZUF3_TextButtonStyle& itemNormal = item.styles[(int)RZUF3_TextButtonState::Normal];
		itemNormal.borderBoxStyle.color = { 0, 0, 0, 0 };
		itemNormal.borderBoxStyle.infillColor = { 0, 0, 0, 0 };
	}
};

struct RZUF3_DropDownItem {
	std::string value;
	std::string text;
	bool useLangFile = true;
};

typedef std::vector<RZUF3_DropDownItem> RZUF3_DropDownItems;