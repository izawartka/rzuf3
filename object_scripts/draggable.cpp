#include "draggable.h"
#include "../game.h"
#include "../events/draggable_update.h"

RZUF3_Draggable::RZUF3_Draggable(RZUF3_DraggableOptions options)
{
	mp_options = options;
}

RZUF3_Draggable::~RZUF3_Draggable()
{

}

void RZUF3_Draggable::init()
{
	m_x = 0.0;
	m_y = 0.0;
	m_scale = mp_options.initialScale;
	m_rect = mp_options.rect;
	m_limitRect = mp_options.limitRect;

	if(mp_options.updateOnInit) update();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, MouseDown);
	_ADD_LISTENER(eventsManager, MouseUp);
	_ADD_LISTENER(eventsManager, MouseMove);
	_ADD_LISTENER(eventsManager, MouseWheel);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, SetRect);
}

void RZUF3_Draggable::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, MouseDown);
	_REMOVE_LISTENER(eventsManager, MouseUp);
	_REMOVE_LISTENER(eventsManager, MouseMove);
	_REMOVE_LISTENER(eventsManager, MouseWheel);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, SetRect);
}

void RZUF3_Draggable::setRect(SDL_Rect rect)
{
	m_rect = rect;

	update();
}

void RZUF3_Draggable::setLimitRect(SDL_Rect limitRect)
{
	m_limitRect = limitRect;

	update();
}

void RZUF3_Draggable::forceUpdate()
{
	update();
}

void RZUF3_Draggable::onMouseDown(RZUF3_MouseDownEvent* event)
{
	if(!mp_options.useDrag) return;

	int x = event->getX();
	int y = event->getY();

	RZUF3_Renderer::screenToRectXY(m_object, m_rect, x, y);
	if (!RZUF3_Renderer::isXYInside(m_rect, x, y)) return;

	if (event->getButton() != mp_options.dragButton) return;

	m_dragging = true;
	m_startCursorX = event->getX();
	m_startCursorY = event->getY();
}

void RZUF3_Draggable::onMouseUp(RZUF3_MouseUpEvent* event)
{
	if (event->getButton() != mp_options.dragButton) return;

	g_game->setRelativeMouseMode(false);
	m_dragging = false;
}

void RZUF3_Draggable::onMouseMove(RZUF3_MouseMoveEvent* event)
{
	if (!m_dragging) return;

	double dx = event->getMovX();
	double dy = event->getMovY();

	if (mp_options.scaleDragFix) {
		dx /= m_scale;
		dy /= m_scale;
	}

	m_x += dx;
	m_y += dy;

	update();

	if (mp_options.lockCursor) {
		g_game->setRelativeMouseMode(true);
	}
}

void RZUF3_Draggable::onMouseWheel(RZUF3_MouseWheelEvent* event)
{
	if (!mp_options.useScale) return;

	double delta = std::pow(2, event->getY() * mp_options.scaleStep);

	double newScale = m_scale * delta;
	if (newScale < mp_options.minScale) newScale = mp_options.minScale;
	if (newScale > mp_options.maxScale) newScale = mp_options.maxScale;
	double fixedDelta = newScale / m_scale;

	m_scale = newScale;
	if (mp_options.centerFix) {
		m_x *= fixedDelta;
		m_y *= fixedDelta;
	}

	update();
}

void RZUF3_Draggable::onSetRect(RZUF3_SetRectEvent* event)
{
	setRect(event->getRect());
}

void RZUF3_Draggable::update()
{	
	if (m_limitRect.w > 0) {
		if (m_x < m_limitRect.x) m_x = m_limitRect.x;
		if (m_x > m_limitRect.x + m_limitRect.w) m_x = m_limitRect.x + m_limitRect.w;
	}

	if (m_limitRect.h > 0) {
		if (m_y < m_limitRect.y) m_y = m_limitRect.y;
		if (m_y > m_limitRect.y + m_limitRect.h) m_y = m_limitRect.y + m_limitRect.h;
	}

	double x = mp_options.centerFix ? (double)m_rect.x + (m_rect.w / 2.0) + m_x : m_x;
	double y = mp_options.centerFix ? (double)m_rect.y + (m_rect.h / 2.0) + m_y : m_y;

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	RZUF3_DraggableUpdateEvent objEvent(x, y, m_scale);
	// spdlog::debug("{} {} {}", x, y, m_scale);
	objEventsManager->dispatchEvent(&objEvent);
}
