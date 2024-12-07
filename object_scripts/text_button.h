#pragma once
#include "../events/events.h"
#include "../event_macros.h"
#include "../renderer.h"
#include "object_scripts.h"
#include "border_box_style.h"

class RZUF3_BorderBox;
class RZUF3_Clickable;
class RZUF3_TextRenderer;

enum class RZUF3_TextButtonState {
	Normal,
	NormalHighlighted,
	Hover,
	Pressed
};

#define RZUF3_TEXT_BUTTON_STATE_COUNT 4

struct RZUF3_TextButtonStyle {
	RZUF3_BorderBoxStyle borderBoxStyle;
	RZUF3_TextStyle textStyle;
	int horizontalPadding = 5;
	int verticalPadding = 5;
	SDL_Rect rect = { 0, 0, 0, 0 };
};

struct RZUF3_TextButtonOptions {
	RZUF3_TextButtonStyle styles[RZUF3_TEXT_BUTTON_STATE_COUNT];
	std::string text;
	bool highlighted = false;
	bool useLangFile = true;
	bool useOnSetRect = true;
	bool useOnDraw = true;
	bool useMouseEvents = true;
	RZUF3_Align alignment = RZUF3_Align_TopLeft;

	bool sp_copyStylesFromNormal = true;
	bool sp_defaultColors = true;
};

class RZUF3_TextButton : public RZUF3_ObjectScript {
public:
	RZUF3_TextButton(RZUF3_TextButtonOptions options);
	~RZUF3_TextButton();

	void init();
	void deinit();

	void setStyle(RZUF3_TextButtonStyle style, RZUF3_TextButtonState state);
	void setText(std::string text);
	void setHighlighted(bool highlighted);
	void setUseLangFile(bool useLangFile);
	void setUseOnSetRect(bool useOnSetRect);
	void setUseOnDraw(bool useOnDraw);
	void setUseMouseEvents(bool useMouseEvents);
	void setAlignment(RZUF3_Align alignment);
	void setRect(SDL_Rect rect); // updates all styles' rects

	void draw();
	void setState(RZUF3_TextButtonState state);

	RZUF3_TextButtonStyle getStyle(RZUF3_TextButtonState state) const { return m_options.styles[(int)state]; }
	bool getHighlighted() const { return m_options.highlighted; }
	bool getUseLangFile() const { return m_options.useLangFile; }
	bool getUseOnSetRect() const { return m_options.useOnSetRect; }
	bool getUseOnDraw() const { return m_options.useOnDraw; }
	bool getUseMouseEvents() const { return m_options.useMouseEvents; }
	RZUF3_Align getAlignment() const { return m_options.alignment; }
	SDL_Rect getRect(RZUF3_TextButtonState state) const { return m_options.styles[(int)state].rect; }

	RZUF3_TextButtonState getState() const { return m_state; }
	SDL_Texture* getTexture();
	SDL_Rect getBorderRect();

protected:
	void onMouseEnter(RZUF3_MouseEnterEvent* event);
	void onMouseLeave(RZUF3_MouseLeaveEvent* event);
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);
	void onSetRect(RZUF3_SetRectEvent* event);
	void onDraw(RZUF3_DrawEvent* event);

	void removeTextRenderer();
	void createTextRenderer();
	void updateTextRenderer();
	void removeClickable();
	void createClickable();
	void updateClickable();
	void removeBorderBox();
	void createBorderBox();
	void updateBorderBox();

	void removeCombinedTexture();
	void createCombinedTexture();

	void resolveSpecialOptions();
	void cacheLangFileText();
	RZUF3_TextButtonStyle* getCurrentStylePtr();

	RZUF3_TextButtonOptions mp_options;

	RZUF3_TextButtonOptions m_options;

	std::string m_cachedText = "";
	RZUF3_TextButtonState m_state = RZUF3_TextButtonState::Normal;
	bool m_hasOnSetRectListener = false;
	bool m_hasOnDrawListener = false;
	bool m_hasMouseEventsListener = false;
	RZUF3_Clickable* m_clickable = nullptr;
	RZUF3_TextRenderer* m_textRenderer = nullptr;
	RZUF3_BorderBox* m_borderBox = nullptr;
	SDL_Texture* m_combinedTexture = nullptr;

	_DECLARE_LISTENER(MouseEnter)
	_DECLARE_LISTENER(MouseLeave)
	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseUp)
	_DECLARE_LISTENER(SetRect)
	_DECLARE_LISTENER(Draw)
};