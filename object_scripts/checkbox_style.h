#pragma once
#include "../common.h"
#include "text_renderer_style.h"
#include "border_box_style.h"

struct RZUF3_CheckboxStyle {
	RZUF3_TextRendererStyle textStyle;
	RZUF3_BorderBoxStyle rectStyle;
	RZUF3_BorderBoxStyle checkStyle;
	int checkSize = 8;
	int rectSize = 16;
	int textOffset = 6;

	void defaultColors() {
		rectStyle.color = { 200, 200, 200, 255 };
		rectStyle.infillColor = { 0, 0, 0, 0 };
		checkStyle.color = { 255, 255, 255, 255 };
		checkStyle.infillColor = { 255, 255, 255, 255 };
		textStyle.color = { 255, 255, 255, 255 };
		textStyle.bgColor = { 0, 0, 0, 0 };
	}
};