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
	m_onHoverCursorId = mp_options.onHoverCursorId;

	if(mp_options.setOnHoverCursor) updateOnHoverCusror();

	m_objEventsManager = m_object->getEventsManager();
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);
	_ADD_LISTENER(eventsManager, MouseDown);
	_ADD_LISTENER(eventsManager, MouseUp);
	_ADD_LISTENER(eventsManager, MouseMove);
	_ADD_LISTENER(m_objEventsManager, SetRect);
}

void RZUF3_Clickable::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Update);
	_REMOVE_LISTENER(eventsManager, MouseDown);
	_REMOVE_LISTENER(eventsManager, MouseUp);
	_REMOVE_LISTENER(eventsManager, MouseMove);
	_REMOVE_LISTENER(m_objEventsManager, SetRect);
	m_objEventsManager = nullptr;

	removeOnHoverCursor();

	m_isMouseOver = false;
	m_isLeftPressed = false;
	m_isRightPressed = false;
	m_lastX = 0;
	m_lastY = 0;
}

void RZUF3_Clickable::setRect(SDL_Rect rect)
{
	m_rect = rect;
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
		RZUF3_MousePressedEvent objEvent(m_lastX, m_lastY, SDL_BUTTON_LEFT);
		m_objEventsManager->dispatchEvent(&objEvent);
	}

	if (m_isRightPressed) {
		RZUF3_MousePressedEvent objEvent(m_lastX, m_lastY, SDL_BUTTON_RIGHT);
		m_objEventsManager->dispatchEvent(&objEvent);
	}
}

void RZUF3_Clickable::onMouseDown(RZUF3_MouseDownEvent* event)
{
	int x = event->getX();
	int y = event->getY();
	RZUF3_Renderer::screenToRectXY(m_object, m_rect, x, y);
	bool inside = RZUF3_Renderer::isXYInside(m_rect, x, y);

	if (inside) {
		if (event->getButton() == SDL_BUTTON_LEFT) m_isLeftPressed = true;
		if (event->getButton() == SDL_BUTTON_RIGHT) m_isRightPressed = true;

		RZUF3_MouseDownEvent objEvent(x, y, event->getButton());
		m_objEventsManager->dispatchEvent(&objEvent);
	}
	else {
		RZUF3_MouseDownOutsideEvent objEvent(x, y, event->getButton());
		m_objEventsManager->dispatchEvent(&objEvent);
	}
}

void RZUF3_Clickable::onMouseUp(RZUF3_MouseUpEvent* event)
{
	int x = event->getX();
	int y = event->getY();
	RZUF3_Renderer::screenToRectXY(m_object, m_rect, x, y);

	if (event->getButton() == SDL_BUTTON_LEFT) m_isLeftPressed = false;
	if (event->getButton() == SDL_BUTTON_RIGHT) m_isRightPressed = false;

	if (!RZUF3_Renderer::isXYInside(m_rect, x, y)) return;

	RZUF3_MouseUpEvent objEvent(x, y, event->getButton());
	m_objEventsManager->dispatchEvent(&objEvent);
}

void RZUF3_Clickable::onMouseMove(RZUF3_MouseMoveEvent* event)
{
	int x = event->getX();
	int y = event->getY();
	RZUF3_Renderer::screenToRectXY(m_object, m_rect, x, y);
	bool isMouseOverNow = RZUF3_Renderer::isXYInside(m_rect, x, y);
	m_lastX = x;
	m_lastY = y;

	if (isMouseOverNow && !m_isMouseOver)
	{
		if (m_onHoverCursor) SDL_SetCursor(m_onHoverCursor);
		RZUF3_MouseEnterEvent objEvent(x, y);
		m_objEventsManager->dispatchEvent(&objEvent);
	}
	else if (!isMouseOverNow && m_isMouseOver)
	{
		SDL_Cursor* defCursor = SDL_GetDefaultCursor();
		SDL_SetCursor(defCursor);
		SDL_FreeCursor(defCursor);
		RZUF3_MouseLeaveEvent objEvent(x, y);
		m_objEventsManager->dispatchEvent(&objEvent);
	}

	m_isMouseOver = isMouseOverNow;
	int movX = event->getMovX() / m_object->getAbsolutePos().scaleX;
	int movY = event->getMovY() / m_object->getAbsolutePos().scaleY;
	RZUF3_MouseMoveEvent objEvent(x, y, movX, movY);
	m_objEventsManager->dispatchEvent(&objEvent);
}

void RZUF3_Clickable::onSetRect(RZUF3_SetRectEvent* event)
{
	setRect(event->getRect());
}

void RZUF3_Clickable::removeOnHoverCursor()
{
	if (m_onHoverCursor == nullptr) return;

	SDL_Cursor* defCursor = SDL_GetDefaultCursor();
	SDL_SetCursor(defCursor);
	SDL_FreeCursor(defCursor);
	SDL_FreeCursor(m_onHoverCursor);
	m_onHoverCursor = nullptr;
}

void RZUF3_Clickable::updateOnHoverCusror()
{
	if(m_onHoverCursor != nullptr) return;
	if(m_onHoverCursorId < 0) return;

	m_onHoverCursor = SDL_CreateSystemCursor(m_onHoverCursorId);

	if (m_isMouseOver) SDL_SetCursor(m_onHoverCursor);
}