#include "window_rect_anchor.h"
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

	m_objEventsManager = m_object->getEventsManager();
}

void RZUF3_WindowRectAnchor::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, WindowResize);
	_REMOVE_LISTENER(eventsManager, Update);

	m_objEventsManager = nullptr;
}

void RZUF3_WindowRectAnchor::setOptions(RZUF3_WindowRectAnchorOptions options)
{
	m_options = options;
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

void RZUF3_WindowRectAnchor::update()
{
	int x1 = m_width * m_options.x1Anchor + m_options.x1Offset;
	int y1 = m_height * m_options.y1Anchor + m_options.y1Offset;

	int x2 = m_width * m_options.x2Anchor + m_options.x2Offset;
	int y2 = m_height * m_options.y2Anchor + m_options.y2Offset;

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
