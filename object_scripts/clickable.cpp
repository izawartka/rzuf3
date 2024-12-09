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
	m_options = mp_options;

	if(m_options.useOnHoverCursor) updateOnHoverCusror();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);
	_ADD_LISTENER(eventsManager, MouseDown);
	_ADD_LISTENER(eventsManager, MouseUp);
	_ADD_LISTENER(eventsManager, MouseMove);

	m_objEventsManager = m_object->getEventsManager();
	setUseOnSetRect(m_options.useOnSetRect);
}

void RZUF3_Clickable::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Update);
	_REMOVE_LISTENER(eventsManager, MouseDown);
	_REMOVE_LISTENER(eventsManager, MouseUp);
	_REMOVE_LISTENER(eventsManager, MouseMove);

	setUseOnSetRect(false);
	m_objEventsManager = nullptr;

	removeOnHoverCursor();

	m_isInside = false;
	m_isLeftPressed = false;
	m_isRightPressed = false;
	m_lastX = 0;
	m_lastY = 0;
}

void RZUF3_Clickable::setRect(SDL_Rect rect)
{
	m_options.rect = rect;
}

void RZUF3_Clickable::setAlignment(RZUF3_Align alignment)
{
	m_options.alignment = alignment;
}

void RZUF3_Clickable::setOnHoverCursor(SDL_SystemCursor id)
{
	removeOnHoverCursor();
	m_options.onHoverCursorId = id;
	updateOnHoverCusror();
}

void RZUF3_Clickable::setUseOnSetRect(bool useOnSetRect)
{
	m_options.useOnSetRect = useOnSetRect;

	if (useOnSetRect && !m_hasSetRectListener) {
		_ADD_LISTENER(m_objEventsManager, SetRect);
		m_hasSetRectListener = true;
	}

	if (!useOnSetRect && m_hasSetRectListener) {
		_REMOVE_LISTENER(m_objEventsManager, SetRect);
		m_hasSetRectListener = false;
	}
}

void RZUF3_Clickable::setInsideOnly(bool insideOnly)
{
	m_options.insideOnly = insideOnly;
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
	SDL_Rect rect = getAlignedRect();
	RZUF3_Renderer::screenToRectXY(m_object, rect, x, y);
	bool inside = RZUF3_Renderer::isXYInside(rect, x, y);

	if (inside || !m_options.insideOnly) {
		if (event->getButton() == SDL_BUTTON_LEFT) m_isLeftPressed = true;
		if (event->getButton() == SDL_BUTTON_RIGHT) m_isRightPressed = true;

		RZUF3_MouseDownEvent objEvent(x, y, event->getButton());
		m_objEventsManager->dispatchEvent(&objEvent);
	}

	if (!inside) {
		RZUF3_MouseDownOutsideEvent objEvent(x, y, event->getButton());
		m_objEventsManager->dispatchEvent(&objEvent);
	}
}

void RZUF3_Clickable::onMouseUp(RZUF3_MouseUpEvent* event)
{
	int x = event->getX();
	int y = event->getY();
	SDL_Rect rect = getAlignedRect();
	RZUF3_Renderer::screenToRectXY(m_object, rect, x, y);

	if (event->getButton() == SDL_BUTTON_LEFT) m_isLeftPressed = false;
	if (event->getButton() == SDL_BUTTON_RIGHT) m_isRightPressed = false;

	bool inside = RZUF3_Renderer::isXYInside(rect, x, y);

	if (inside || !m_options.insideOnly) {
		RZUF3_MouseUpEvent objEvent(x, y, event->getButton());
		m_objEventsManager->dispatchEvent(&objEvent);
	}

	if (!inside) {
		RZUF3_MouseUpOutsideEvent objEvent(x, y, event->getButton());
		m_objEventsManager->dispatchEvent(&objEvent);
	}
}

void RZUF3_Clickable::onMouseMove(RZUF3_MouseMoveEvent* event)
{
	int x = event->getX();
	int y = event->getY();
	SDL_Rect rect = getAlignedRect();
	RZUF3_Renderer::screenToRectXY(m_object, rect, x, y);
	bool insideNow = RZUF3_Renderer::isXYInside(rect, x, y);
	m_lastX = x;
	m_lastY = y;

	if (insideNow && !m_isInside)
	{
		if (m_onHoverCursor) {
			SDL_SetCursor(m_onHoverCursor);
		}

		RZUF3_MouseEnterEvent objEvent(x, y);
		m_objEventsManager->dispatchEvent(&objEvent);
	}
	else if (!insideNow && m_isInside)
	{
		if (m_onHoverCursor) {
			SDL_Cursor* defCursor = SDL_GetDefaultCursor();
			SDL_SetCursor(defCursor);
			SDL_FreeCursor(defCursor);
		}

		RZUF3_MouseLeaveEvent objEvent(x, y);
		m_objEventsManager->dispatchEvent(&objEvent);
	}

	if (!insideNow && !m_isInside && m_options.insideOnly) {
		m_isInside = insideNow;
		return;
	}

	m_isInside = insideNow;
	int movX = event->getMovX() / m_object->getAbsolutePos().scaleX;
	int movY = event->getMovY() / m_object->getAbsolutePos().scaleY;

	RZUF3_MouseMoveEvent objEvent(x, y, movX, movY);
	m_objEventsManager->dispatchEvent(&objEvent);
}

void RZUF3_Clickable::onSetRect(RZUF3_SetRectEvent* event)
{
	if (!m_options.useOnSetRect) return;
	setRect(event->getRect());
}

void RZUF3_Clickable::removeOnHoverCursor()
{
	if (m_onHoverCursor == nullptr) return;

	if (m_isInside) {
		SDL_Cursor* defCursor = SDL_GetDefaultCursor();
		SDL_SetCursor(defCursor);
		SDL_FreeCursor(defCursor);
	}

	SDL_FreeCursor(m_onHoverCursor);
	m_onHoverCursor = nullptr;
}

void RZUF3_Clickable::updateOnHoverCusror()
{
	if(m_onHoverCursor != nullptr) return;
	if(m_options.onHoverCursorId < 0) return;

	m_onHoverCursor = SDL_CreateSystemCursor(m_options.onHoverCursorId);

	if (m_isInside) {
		SDL_SetCursor(m_onHoverCursor);
	}
}

SDL_Rect RZUF3_Clickable::getAlignedRect()
{
	SDL_Rect rect = m_options.rect;
	RZUF3_Renderer::alignRect(rect, m_options.alignment);
	return rect;
}
