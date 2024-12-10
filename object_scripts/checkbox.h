#pragma once
#include "../events/events.h"
#include "../event_macros.h"
#include "object_scripts.h"
#include "checkbox_style.h"

class RZUF3_TextRenderer;
class RZUF3_BorderBox;
class RZUF3_Clickable;

struct RZUF3_CheckboxOptions {
	RZUF3_CheckboxStyle style;
	std::string labelText;
	bool checked = false;
	bool useOnDraw = true;
	bool useMouseEvents = true;
	bool useLangFile = true;
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
};

class RZUF3_Checkbox : public RZUF3_ObjectScript {
public:
	RZUF3_Checkbox(std::string fontFilepath, std::string text);
	RZUF3_Checkbox(RZUF3_CheckboxOptions options);
	~RZUF3_Checkbox();

	void init();
	void deinit();

	void setStyle(RZUF3_CheckboxStyle style);
	void setLabelText(std::string text);
	void setChecked(bool checked);
	void setUseOnDraw(bool useOnDraw);
	void setUseMouseEvents(bool useMouseEvents);
	void setUseLangFile(bool useLangFile);
	void setAlignment(RZUF3_Align alignment);

	void draw();

	RZUF3_CheckboxStyle getStyle() const { return m_options.style; }
	std::string getLabelText() const { return m_options.labelText; }
	bool getChecked() const { return m_options.checked; }
	bool getUseOnDraw() const { return m_options.useOnDraw; }
	bool getUseMouseEvents() const { return m_options.useMouseEvents; }
	bool getUseLangFile() const { return m_options.useLangFile; }
	RZUF3_Align getAlignment() const { return m_options.alignment; }

	SDL_Rect getRect() const;

protected:
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);
	void onMouseUpOutside(RZUF3_MouseUpOutsideEvent* event);
	void onUISetValue(RZUF3_UISetValueEvent* event);
	void onDraw(RZUF3_DrawEvent* event);

	int getHalfRectTextLineHeightDiff() const;
	void sendValueChangeEvent();

	void removeTextRenderer();
	void createTextRenderer();
	void updateTextRenderer();
	void removeBorderBoxes();
	void createBorderBoxes();
	void updateBorderBoxes();
	void removeClickable();
	void createClickable();
	void updateClickable();

	void removeCombinedTexture();
	void createCombinedTexture();

	RZUF3_CheckboxOptions mp_options;

	RZUF3_CheckboxOptions m_options;
	bool m_pressed = false;
	bool m_hasOnDrawListener = false;
	bool m_hasMouseEventsListener = false;
	RZUF3_TextRenderer* m_textRenderer = nullptr;
	RZUF3_BorderBox* m_rectBorderBox = nullptr;
	RZUF3_BorderBox* m_checkBorderBox = nullptr;
	RZUF3_Clickable* m_clickable = nullptr;
	SDL_Texture* m_combinedTexture = nullptr;
	bool m_subScriptsOptionsChanged = false;

	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseUp)
	_DECLARE_LISTENER(MouseUpOutside)
	_DECLARE_LISTENER(UISetValue)
};