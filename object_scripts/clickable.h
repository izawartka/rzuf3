#pragma once
#include "../events/update.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_UpdateEvent;
class RZUF3_MouseDownEvent;
class RZUF3_MouseUpEvent;
class RZUF3_MouseMoveEvent;
class RZUF3_SetRectEvent;

struct RZUF3_ClickableOptions {
	SDL_Rect rect = { 0, 0, 0, 0 };
	bool useOnHoverCursor = true;
	bool useOnSetRect = true;
	bool insideOnly = true; // should only trigger MouseDown, MouseMove and MouseUp events when mouse is inside the rect
	SDL_SystemCursor onHoverCursorId = SDL_SYSTEM_CURSOR_HAND;
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
};

class RZUF3_Clickable : public RZUF3_ObjectScript {
public:
	RZUF3_Clickable();
	RZUF3_Clickable(SDL_Rect rect);
	RZUF3_Clickable(RZUF3_ClickableOptions options);
	~RZUF3_Clickable();

	void init();
	void deinit();

	void setRect(SDL_Rect rect);
	void setAlignment(RZUF3_Align alignment);
	void setOnHoverCursor(SDL_SystemCursor id);
	void setUseOnSetRect(bool useOnSetRect);
	void setInsideOnly(bool insideOnly);

	bool isInside() const { return m_isInside; }

	SDL_Rect getRect() const { return m_options.rect; }
	RZUF3_Align getAlignment() const { return m_options.alignment; }
	SDL_SystemCursor getOnHoverCursor() const { return m_options.onHoverCursorId; }
	bool getUseOnSetRect() const { return m_options.useOnSetRect; }
	bool getInsideOnly() const { return m_options.insideOnly; }
	void getLastMousePos(int& x, int& y) const { x = m_lastX; y = m_lastY; }

protected:
	void onUpdate(RZUF3_UpdateEvent* event);
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);
	void onMouseMove(RZUF3_MouseMoveEvent* event);
	void onSetRect(RZUF3_SetRectEvent* event);

	void removeOnHoverCursor();
	void updateOnHoverCusror();
	SDL_Rect getAlignedRect();

	RZUF3_ClickableOptions mp_options;

	RZUF3_ClickableOptions m_options;
	bool m_isInside = false;
	bool m_isLeftPressed = false;
	bool m_isRightPressed = false;
	int m_lastX = 0;
	int m_lastY = 0;

	RZUF3_EventsManager* m_objEventsManager = nullptr;
	SDL_Cursor* m_onHoverCursor = nullptr;
	bool m_hasSetRectListener = false;

	_DECLARE_LISTENER(Update)
	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseUp)
	_DECLARE_LISTENER(MouseMove)
	_DECLARE_LISTENER(SetRect)
};