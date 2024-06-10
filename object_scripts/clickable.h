#pragma once
#include "../common.h"
#include "../events/update.h"
#include "../object_script.h"
#include "../object_script_macros.h"

class RZUF3_ObjectScript;
class RZUF3_UpdateEvent;

typedef std::function<void(RZUF3_MouseDownEvent*)> RZUF3_ClickableCallback;

struct RZUF3_ClickableOptions {
	SDL_Rect rect;
	SDL_SystemCursor onHoverCursorId = SDL_SYSTEM_CURSOR_HAND;
	RZUF3_ClickableCallback mouseDownCallback = nullptr;
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
	void setMouseDownCallback(RZUF3_ClickableCallback mouseDownCallback);
	void setOnHoverCursor(SDL_SystemCursor id);

protected:
	void onUpdate(RZUF3_UpdateEvent* event);
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);
	void onMouseMove(RZUF3_MouseMoveEvent* event);

	void removeOnHoverCursor();
	void updateOnHoverCusror();

	void screenToRectXY(int &x, int &y) const;
	bool isXYInside(int x, int y) const;

	RZUF3_ClickableOptions mp_options;

	SDL_Rect m_rect = { 0, 0, 0, 0 };
	bool m_isMouseOver = false;
	bool m_isLeftPressed = false;
	bool m_isRightPressed = false;
	int m_lastX = 0;
	int m_lastY = 0;

	RZUF3_ClickableCallback m_mouseDownCallback = nullptr;
	RZUF3_EventsManager* m_objEventsManager = nullptr;
	SDL_SystemCursor m_onHoverCursorId = SDL_SYSTEM_CURSOR_HAND;
	SDL_Cursor* m_onHoverCursor = nullptr;

	_DECLARE_LISTENER(Update)
	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseUp)
	_DECLARE_LISTENER(MouseMove)
};