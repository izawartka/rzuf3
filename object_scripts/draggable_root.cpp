#include "draggable_root.h"
#include "../game.h"

RZUF3_DraggableRoot::RZUF3_DraggableRoot(RZUF3_DraggableRootOptions options)
{
	mp_options = options;
}

RZUF3_DraggableRoot::~RZUF3_DraggableRoot()
{

}

void RZUF3_DraggableRoot::init()
{
	m_offX = 0;
	m_offY = 0;
	m_rect = mp_options.rect;
	m_limitRect = mp_options.limitRect;

	linkContent();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, MouseDown);
	_ADD_LISTENER(eventsManager, MouseUp);
	_ADD_LISTENER(eventsManager, MouseMove);
	_ADD_LISTENER(eventsManager, MouseWheel);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, SetRect);
}

void RZUF3_DraggableRoot::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, MouseDown);
	_REMOVE_LISTENER(eventsManager, MouseUp);
	_REMOVE_LISTENER(eventsManager, MouseMove);
	_REMOVE_LISTENER(eventsManager, MouseWheel);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, SetRect);

	m_contentObj = nullptr;
}

void RZUF3_DraggableRoot::setRect(SDL_Rect rect)
{
	m_rect = rect;

	updateContentPosition();
}

void RZUF3_DraggableRoot::setLimitRect(SDL_Rect limitRect)
{
	m_limitRect = limitRect;

	updateContentPosition();
}

void RZUF3_DraggableRoot::onMouseDown(RZUF3_MouseDownEvent* event)
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

void RZUF3_DraggableRoot::onMouseUp(RZUF3_MouseUpEvent* event)
{
	if (event->getButton() != mp_options.dragButton) return;

	g_game->setRelativeMouseMode(false);
	m_dragging = false;
}

void RZUF3_DraggableRoot::onMouseMove(RZUF3_MouseMoveEvent* event)
{
	if (!m_dragging) return;

	m_offX += event->getMovX();
	m_offY += event->getMovY();

	updateContentPosition();

	if (mp_options.lockCursor) {
		g_game->setRelativeMouseMode(true);
	}
}

void RZUF3_DraggableRoot::onMouseWheel(RZUF3_MouseWheelEvent* event)
{
	if (!mp_options.useScale) return;

	double delta = std::pow(1.1, event->getY());

	double newScale = m_scale * delta;
	if (newScale < mp_options.minScale) newScale = mp_options.minScale;
	if (newScale > mp_options.maxScale) newScale = mp_options.maxScale;
	double fixedDelta = newScale / m_scale;

	m_scale = newScale;
	m_offX *= fixedDelta;
	m_offY *= fixedDelta;

	updateContentPosition();
}

void RZUF3_DraggableRoot::onSetRect(RZUF3_SetRectEvent* event)
{
	setRect(event->getRect());
}

void RZUF3_DraggableRoot::linkContent()
{
	m_contentObj = g_scene->getObject(mp_options.contentObjName);
	if (m_contentObj == nullptr) {
		spdlog::error("Draggable content object not found");
		return;
	}

	updateContentPosition();
}

void RZUF3_DraggableRoot::updateContentPosition()
{	
	if (m_limitRect.w > 0) {
		if (m_offX < m_limitRect.x) m_offX = m_limitRect.x;
		if (m_offX > m_limitRect.x + m_limitRect.w) m_offX = m_limitRect.x + m_limitRect.w;
	}

	if (m_limitRect.h > 0) {
		if (m_offY < m_limitRect.y) m_offY = m_limitRect.y;
		if (m_offY > m_limitRect.y + m_limitRect.h) m_offY = m_limitRect.y + m_limitRect.h;
	}

	m_contentObj->setPos({
		m_rect.x + (m_rect.w / 2) + m_offX,
		m_rect.y + (m_rect.h / 2) + m_offY,
		m_scale,
		m_scale
	});
}
