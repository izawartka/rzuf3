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
	SDL_Rect rect;
	bool setOnHoverCursor = true;
	SDL_SystemCursor onHoverCursorId = SDL_SYSTEM_CURSOR_HAND;
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
	void setOnHoverCursor(SDL_SystemCursor id);

	SDL_Rect getRect() const { return m_rect; }

protected:
	void onUpdate(RZUF3_UpdateEvent* event);
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);
	void onMouseMove(RZUF3_MouseMoveEvent* event);
	void onSetRect(RZUF3_SetRectEvent* event);

	void removeOnHoverCursor();
	void updateOnHoverCusror();

	RZUF3_ClickableOptions mp_options;

	SDL_Rect m_rect = { 0, 0, 0, 0 };
	bool m_isMouseOver = false;
	bool m_isLeftPressed = false;
	bool m_isRightPressed = false;
	int m_lastX = 0;
	int m_lastY = 0;

	RZUF3_EventsManager* m_objEventsManager = nullptr;
	SDL_SystemCursor m_onHoverCursorId = SDL_SYSTEM_CURSOR_HAND;
	SDL_Cursor* m_onHoverCursor = nullptr;

	_DECLARE_LISTENER(Update)
	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseUp)
	_DECLARE_LISTENER(MouseMove)
	_DECLARE_LISTENER(SetRect)
};