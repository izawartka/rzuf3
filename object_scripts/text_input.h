#pragma once
#include "../events/draw.h"
#include "../event_macros.h"
#include "../renderer.h"
#include "../text_style.h"
#include "object_scripts.h"
#include "border_box_style.h"

class RZUF3_ObjectScript;
class RZUF3_Renderer;
class RZUF3_Clickable;
class RZUF3_TextSelRenderer;
class RZUF3_DrawEvent;
class RZUF3_MouseDownEvent;
class RZUF3_MouseDownOutsideEvent;
class RZUF3_UISetStringValueEvent;
class RZUF3_Timer;
class RZUF3_TextInputFocusEvent;
class RZUF3_TextInputEvent;
class RZUF3_TextEditingEvent;
class RZUF3_BorderBox;

struct RZUF3_TextInputStyle {
	RZUF3_TextStyle textStyle;
	RZUF3_BorderBoxStyle borderBoxStyle;
	SDL_Color blinkColor = { 255, 255, 255, 255 };
	SDL_Rect rect = { 0, 0, 128, 0 };
	int horizontalPadding = 5;
	int verticalPadding = 5;
	double blinkTime = 500.0;
};

struct RZUF3_TextInputOptions {
	std::string text;
	bool multiline = false;
	int maxChars = 0;
	RZUF3_TextInputStyle style;
	RZUF3_TextInputStyle focusedStyle;
};

class RZUF3_TextInput : public RZUF3_ObjectScript {
public:
	RZUF3_TextInput(RZUF3_TextInputOptions options);
	~RZUF3_TextInput();

	void init();
	void deinit();

	void setMaxChars(int maxChars);
	void setStyle(RZUF3_TextInputStyle style, bool focused);
	void setText(std::string text);
	void setMultiline(bool multiline);
	void setFocused(bool focused);
	void setCursorPos(int pos);
	void setCursorPosFromXY(int x, int y);
	void addText(std::string text, bool noNewLineCheck = false);
	void removeText(bool backspace);

	RZUF3_TextInputStyle getStyle(bool focused) const { return focused ? m_options.focusedStyle : m_options.style; }
	std::string getText() const { return m_text; }
	bool getMultiline() const { return m_options.multiline; }
	bool getFocused() const { return m_isFocused; }
	SDL_Rect getBorderRect(bool focused);

protected:
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseDownOutside(RZUF3_MouseDownOutsideEvent* event);
	void onUISetStringValue(RZUF3_UISetStringValueEvent* event);
	void onSetRect(RZUF3_SetRectEvent* event);
	void onTimer(RZUF3_TimerEvent* event);
	void onDraw(RZUF3_DrawEvent* event);
	void onTextInputFocus(RZUF3_TextInputFocusEvent* event);
	void onTextInput(RZUF3_TextInputEvent* event);
	void onTextSelectionChange(RZUF3_TextSelectionChangeEvent* event);
	void onKeyDown(RZUF3_KeyDownEvent* event);
	void onKeyUp(RZUF3_KeyUpEvent* event);

	void controlledSetText(std::string text, bool noNewLineCheck = false);
	void updateTextRendererStyle();

	void removeTextRenderer();
	void createTextRenderer();
	void updateTextRenderer();
	void removeBorderBox();
	void createBorderBox();
	void updateBorderBox();
	void removeClickable();
	void createClickable();
	void updateClickable();

	RZUF3_TextInputStyle* getCurrentStylePtr();
	void updateCursorRectAndScroll(bool noAutoscroll = false);
	void setScroll(int x, int y);

	RZUF3_TextInputOptions mp_options;

	RZUF3_TextInputOptions m_options;

	std::string m_text;
	bool m_isFocused = false;
	bool m_isBlinking = false;
	int m_scrollX = 0;
	int m_scrollY = 0;
	SDL_Rect m_cachedCursorRect = { 0, 0, 0, 0 };
	bool m_isCtrlDown = false;
	bool m_isShiftDown = false;

	RZUF3_Clickable* m_clickable = nullptr;
	RZUF3_TextSelRenderer* m_textRenderer = nullptr;
	RZUF3_BorderBox* m_borderBox = nullptr;
	RZUF3_Timer* m_blinkTimer = nullptr;

	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseDownOutside)
	_DECLARE_LISTENER(UISetStringValue)
	_DECLARE_LISTENER(SetRect)
	_DECLARE_LISTENER(Timer)
	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(TextInputFocus)
	_DECLARE_LISTENER(TextInput)
	_DECLARE_LISTENER(TextSelectionChange)
	_DECLARE_LISTENER(KeyDown)
	_DECLARE_LISTENER(KeyUp)
};