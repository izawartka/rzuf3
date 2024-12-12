#pragma once
#include "../events/events.h"
#include "../event_macros.h"
#include "text_sel_renderer_style.h"
#include "text_renderer.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

struct RZUF3_TextSelRendererOptions {
	std::string text;
	SDL_Rect dstRect = { 0, 0, 0, 0 };
	SDL_Rect cropRect = { 0, 0, 0, 0 };
	RZUF3_TextSelStyle style;
	bool useOnDraw = true;
	bool useMouseEvents = true;
	bool useKeyboardEvents = true;
	bool useOnHoverCursor = true;
	bool useLangFile = true;
	bool forceFocus = false;
	bool preventCtrlX = false;
	int wrapLength = 0; // doesn't work yet
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
};

class RZUF3_TextSelRenderer : public RZUF3_ObjectScript {
public:
	RZUF3_TextSelRenderer(std::string fontFilepath, std::string text);
	RZUF3_TextSelRenderer(RZUF3_TextSelRendererOptions options);
	~RZUF3_TextSelRenderer();

	void init();
	void deinit();

	void setFontFilepath(std::string fontFilepath);
	void setDstPos(int x, int y);
	void setDstSize(int width, int height);
	void setOffset(int offsetX, int offsetY);
	void setMaxSize(int maxWidth, int maxHeight);
	void setText(std::string text, bool resetSelection = true);
	void setStyle(RZUF3_TextSelStyle style);
	void setUseLangFile(bool useLangFile);
	void setUseOnDraw(bool useOnDraw);
	void setUseMouseEvents(bool useMouseEvents);
	void setUseKeyboardEvents(bool useKeyboardEvents);
	void setWrapLength(int wrapLength);
	void setAlignment(RZUF3_Align alignment);
	void setForceFocus(bool forceFocus);
	void setFocused(bool focused) const;
	void setSelection(int start, int end);
	void setSelectionEndFromXY(int x, int y, bool alsoStart);
	int pointToCharIndex(int x, int y);
	bool charIndexToPoint(int index, int& x, int& y);
	void draw();

	SDL_Texture* getTexture() const;
	RZUF3_TextSelStyle getStyle() const;
	int getWidth() const;
	int getHeight() const;
	bool getUseOnDraw() const;
	bool getUseMouseEvents() const;
	bool getUseKeyboardEvents() const;
	int getFontAscent() const;
	int getFontDescent() const;
	int getFontHeight() const;
	void getSelection(int& start, int& end) const;
	int getSelectionStart() const;
	int getSelectionEnd() const;
	std::string getSelectedText() const;

protected:
	void onDraw(RZUF3_DrawEvent* event);
	void onLangChange(RZUF3_LangChangeEvent* event);
	void onTextSelFocus(RZUF3_TextSelFocusEvent* event);
	void onKeyDown(RZUF3_KeyDownEvent* event);
	void onKeyUp(RZUF3_KeyUpEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseMove(RZUF3_MouseMoveEvent* event);

	int getNeighbourCharIndexVertically(int index, bool up);
	RZUF3_TextRendererStyle getSelectedTextStyle() const;

	void removeTextRenderers();
	void createTextRenderers();
	void removeCombinedTexture();
	void createCombinedTexture();
	void cacheLangFileText();
	void cacheRealRects();
	void updateSelRects();
	void sendSelectionChangeEvent();

	RZUF3_TextSelRendererOptions mp_options;

	RZUF3_TextSelRendererOptions m_options;

	int m_selStart = 0;
	int m_selEnd = 0;
	bool m_isSelecting = false;
	bool m_isShiftDown = false;
	bool m_isCtrlDown = false;
	bool m_isFocused = false;
	std::string m_cachedText = "";
	SDL_Rect m_origSize = { 0, 0, 0, 0 };
	RZUF3_TextRenderer* m_textRenderer = nullptr;
	RZUF3_TextRenderer* m_textRendererSelected = nullptr;
	RZUF3_Clickable* m_clickable = nullptr;
	SDL_Texture* m_combinedTexture = nullptr;
	bool m_hasOnDrawListener = false;
	bool m_hasMouseEventsListener = false;
	bool m_hasKeyboardEventsListener = false;
	bool m_hasOnLangChangeListener = false;
	SDL_Rect m_realSrcRect = { 0, 0, 0, 0 };
	SDL_Rect m_realDstRect = { 0, 0, 0, 0 };
	std::vector<SDL_Rect> m_selRects;
	
	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(LangChange)
	_DECLARE_LISTENER(TextSelFocus)
	_DECLARE_LISTENER(KeyDown)
	_DECLARE_LISTENER(KeyUp)
	_DECLARE_LISTENER(MouseUp) // global
	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseMove)
};