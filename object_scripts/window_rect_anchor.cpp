#include "window_rect_anchor.h"
#include "../events/set_anchor.h"
#include "../events/get_anchor.h"
#include "../game.h"

RZUF3_WindowRectAnchor::RZUF3_WindowRectAnchor(RZUF3_WindowRectAnchorOptions options)
{
	mp_options = options;
}

RZUF3_WindowRectAnchor::~RZUF3_WindowRectAnchor()
{

}

void RZUF3_WindowRectAnchor::init()
{
	m_options = mp_options;
	m_isFirstUpdate = true;

	g_game->getWindowSize(&m_width, &m_height);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, WindowResize);
	_ADD_LISTENER(eventsManager, Update);
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, SetAnchor);
	_ADD_LISTENER(objEventsManager, GetAnchor);

	m_objEventsManager = m_object->getEventsManager();
}

void RZUF3_WindowRectAnchor::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, WindowResize);
	_REMOVE_LISTENER(eventsManager, Update);
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, SetAnchor);
	_REMOVE_LISTENER(objEventsManager, GetAnchor);

	m_objEventsManager = nullptr;
}

void RZUF3_WindowRectAnchor::setOptions(RZUF3_WindowRectAnchorOptions options)
{
	m_options = options;
	update();
}

void RZUF3_WindowRectAnchor::setAnchor(RZUF3_Anchor anchor, int index)
{
	if (index < 0 || index >= 2) {
		spdlog::error("WindowRectAnchor: index out of bounds");
		return;
	}

	m_options.anchors[index] = anchor;
	update();
}

RZUF3_WindowRectAnchorOptions RZUF3_WindowRectAnchor::getOptions()
{
	return m_options;
}

void RZUF3_WindowRectAnchor::onUpdate(RZUF3_UpdateEvent* event)
{
	if (!m_isFirstUpdate) return;
	m_isFirstUpdate = false;

	update();
}

void RZUF3_WindowRectAnchor::onWindowResize(RZUF3_WindowResizeEvent* event)
{
	m_width = event->GetWidth();
	m_height = event->GetHeight();

	update();
}

void RZUF3_WindowRectAnchor::onSetAnchor(RZUF3_SetAnchorEvent* event)
{
	setAnchor(event->getAnchor(), event->getAnchorIndex());
}

void RZUF3_WindowRectAnchor::onGetAnchor(RZUF3_GetAnchorEvent* event) const
{
	int index = event->getAnchorIndex();

	if (index < 0 || index >= 2) {
		return;
	}

	RZUF3_Anchor anchor = m_options.anchors[index];

	event->setAnchor(anchor);
}

void RZUF3_WindowRectAnchor::update()
{
	int x1 = m_width * m_options.anchors[0].xAnchor + m_options.anchors[0].xOffset;
	int y1 = m_height * m_options.anchors[0].yAnchor + m_options.anchors[0].yOffset;

	int x2 = m_width * m_options.anchors[1].xAnchor + m_options.anchors[1].xOffset;
	int y2 = m_height * m_options.anchors[1].yAnchor + m_options.anchors[1].yOffset;

	SDL_Rect newRect = {
		x1,
		y1,
		x2 - x1,
		y2 - y1
	};

	// spdlog::debug("WindowRectAnchor: {} {} {} {}", newRect.x, newRect.y, newRect.w, newRect.h);

	RZUF3_SetRectEvent objEvent(newRect);
	m_objEventsManager->dispatchEvent(&objEvent);
}
