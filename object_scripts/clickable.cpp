#include "clickable.h"

RZUF3_Clickable::RZUF3_Clickable()
{

}

RZUF3_Clickable::RZUF3_Clickable(SDL_Rect rect)
{
	mp_options.rect = rect;
}

RZUF3_Clickable::RZUF3_Clickable(RZUF3_ClickableOptions options)
{
	mp_options = options;
}

RZUF3_Clickable::~RZUF3_Clickable()
{
}

void RZUF3_Clickable::init()
{
	m_rect = mp_options.rect;
	m_mouseDownCallback = mp_options.mouseDownCallback;
	m_onHoverCursorId = mp_options.onHoverCursorId;
	m_isMouseOver = false;
	m_isLeftPressed = false;
	m_isRightPressed = false;
	m_lastRelX = 0;
	m_lastRelY = 0;
	m_objEventsManager = getObject()->getEventsManager();
	m_onHoverCursor = nullptr;

	updateOnHoverCusror();

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);
	_ADD_LISTENER(eventsManager, MouseDown);
	_ADD_LISTENER(eventsManager, MouseUp);
	_ADD_LISTENER(eventsManager, MouseMove);
}

void RZUF3_Clickable::deinit()
{
	removeOnHoverCursor();

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Update);
	_REMOVE_LISTENER(eventsManager, MouseDown);
	_REMOVE_LISTENER(eventsManager, MouseUp);
	_REMOVE_LISTENER(eventsManager, MouseMove);
}

void RZUF3_Clickable::setRect(SDL_Rect rect)
{
	m_rect = rect;
}

void RZUF3_Clickable::setMouseDownCallback(RZUF3_ClickableCallback mouseDownCallback)
{
	m_mouseDownCallback = mouseDownCallback;
} 

void RZUF3_Clickable::setOnHoverCursor(SDL_SystemCursor id)
{
	removeOnHoverCursor();
	m_onHoverCursorId = id;
	updateOnHoverCusror();
}

void RZUF3_Clickable::onUpdate(RZUF3_UpdateEvent* event)
{
	if (m_isLeftPressed) {
		RZUF3_MousePressedEvent objEvent(m_lastRelX, m_lastRelY, SDL_BUTTON_LEFT);
		m_objEventsManager->dispatchEvent(&objEvent);
	}

	if (m_isRightPressed) {
		RZUF3_MousePressedEvent objEvent(m_lastRelX, m_lastRelY, SDL_BUTTON_RIGHT);
		m_objEventsManager->dispatchEvent(&objEvent);
	}
}

void RZUF3_Clickable::onMouseDown(RZUF3_MouseDownEvent* event)
{
	RZUF3_Pos relPos = getRelPos(event->getX(), event->getY());
	if (!isPosInside(relPos)) return;

	RZUF3_MouseDownEvent objEvent(relPos.x, relPos.y, event->getButton());
	m_objEventsManager->dispatchEvent(&objEvent);

	if (event->getButton() == SDL_BUTTON_LEFT) m_isLeftPressed = true;
	if (event->getButton() == SDL_BUTTON_RIGHT) m_isRightPressed = true;

	if (m_mouseDownCallback) m_mouseDownCallback(event);
}

void RZUF3_Clickable::onMouseUp(RZUF3_MouseUpEvent* event)
{
	RZUF3_Pos relPos = getRelPos(event->getX(), event->getY());

	if (event->getButton() == SDL_BUTTON_LEFT) m_isLeftPressed = false;
	if (event->getButton() == SDL_BUTTON_RIGHT) m_isRightPressed = false;

	if (!isPosInside(relPos)) return;

	RZUF3_MouseUpEvent objEvent(relPos.x, relPos.y, event->getButton());
	m_objEventsManager->dispatchEvent(&objEvent);
}

void RZUF3_Clickable::onMouseMove(RZUF3_MouseMoveEvent* event)
{
	RZUF3_Pos relPos = getRelPos(event->getX(), event->getY());
	bool isMouseOverNow = isPosInside(relPos);
	m_lastRelX = relPos.x;
	m_lastRelY = relPos.y;

	if (isMouseOverNow && !m_isMouseOver)
	{
		if (m_onHoverCursor) SDL_SetCursor(m_onHoverCursor);
		RZUF3_MouseEnterEvent objEvent(relPos.x, relPos.y);
		m_objEventsManager->dispatchEvent(&objEvent);
	}
	else if (!isMouseOverNow && m_isMouseOver)
	{
		SDL_Cursor* defCursor = SDL_GetDefaultCursor();
		SDL_SetCursor(defCursor);
		SDL_FreeCursor(defCursor);
		RZUF3_MouseLeaveEvent objEvent(relPos.x, relPos.y);
		m_objEventsManager->dispatchEvent(&objEvent);
	}

	m_isMouseOver = isMouseOverNow;
	RZUF3_MouseMoveEvent objEvent(relPos.x, relPos.y, event->getMovX(), event->getMovY());
	m_objEventsManager->dispatchEvent(&objEvent);
}

void RZUF3_Clickable::removeOnHoverCursor()
{
	if (m_onHoverCursor == nullptr) return;

	SDL_FreeCursor(m_onHoverCursor);
	m_onHoverCursor = nullptr;
}

void RZUF3_Clickable::updateOnHoverCusror()
{
	if(m_onHoverCursor != nullptr) return;
	if(m_onHoverCursorId < 0) return;

	m_onHoverCursor = SDL_CreateSystemCursor(m_onHoverCursorId);

}

RZUF3_Pos RZUF3_Clickable::getRelPos(int x, int y) const
{
	RZUF3_Pos objectPos = m_object->getPos();
	int relX = x - m_rect.x - objectPos.x;
	int relY = y - m_rect.y - objectPos.y;

	return RZUF3_Pos(relX, relY);
}

bool RZUF3_Clickable::isPosInside(RZUF3_Pos relPos) const
{
	if (relPos.x < 0 || relPos.x >= m_rect.w) return false;
	if (relPos.y < 0 || relPos.y >= m_rect.h) return false;

	return true;
}
