#pragma once
#include "../common.h"
#include "../events/update.h"
#include "../object_script.h"
#include "../object_script_macros.h"
#include "../renderer.h"
#include "clickable.h"
#include "text_renderer.h"

class RZUF3_ObjectScript;

struct RZUF3_CheckboxStyle {
	SDL_Color textColor = { 255, 255, 255, 255 };
	SDL_Color borderColor = { 255, 255, 255, 255 };
	SDL_Color checkColor = { 255, 255, 255, 255 };
	SDL_Color bgColor = { 0, 0, 0, 255 };
	int textSize = 14;
	int textStyle = TTF_STYLE_NORMAL;
	int borderWidth = 2;
	int checkOffset = 2;
	int rectSize = 16;
	int textOffset = 6;
};

struct RZUF3_CheckboxOptions {
	std::string fontFilepath;
	std::string text;
	bool checked = false;
	RZUF3_CheckboxStyle style;
};

class RZUF3_Checkbox : public RZUF3_ObjectScript {
public:
	RZUF3_Checkbox(std::string fontFilepath, std::string text);
	RZUF3_Checkbox(RZUF3_CheckboxOptions options);
	~RZUF3_Checkbox();

	void init();
	void deinit();

	void setStyle(RZUF3_CheckboxStyle style);
	void setChecked(bool checked);
	void setText(std::string text);

	RZUF3_CheckboxStyle getStyle() const;
	bool getChecked() const;

protected:
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onUISetValue(RZUF3_UISetValueEvent* event);
	void onDraw(RZUF3_DrawEvent* event);

	void updateStyle();

	RZUF3_CheckboxOptions mp_options;

	RZUF3_CheckboxStyle m_style;
	bool m_checked = false;
	RZUF3_Renderer* m_renderer = nullptr;

	RZUF3_Clickable* m_clickable = nullptr;
	RZUF3_TextRenderer* m_textRenderer = nullptr;

	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(UISetValue)
	_DECLARE_LISTENER(Draw)
};