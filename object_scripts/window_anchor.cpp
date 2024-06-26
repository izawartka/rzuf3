#include "window_anchor.h"
#include "../game.h"
#include "../events/window_resize.h"
#include "../events/set_anchor.h"
#include "../events/get_anchor.h"

RZUF3_WindowAnchor::RZUF3_WindowAnchor(double xAnchor, double yAnchor)
{
	mp_anchor.xAnchor = xAnchor;
	mp_anchor.yAnchor = yAnchor;
}

RZUF3_WindowAnchor::RZUF3_WindowAnchor(RZUF3_Anchor anchor)
{
	mp_anchor = anchor;
}

RZUF3_WindowAnchor::~RZUF3_WindowAnchor()
{

}

void RZUF3_WindowAnchor::init()
{
	m_anchor = mp_anchor;

	g_game->getWindowSize(&m_width, &m_height);
	update();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, WindowResize);
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, SetAnchor);
	_ADD_LISTENER(objEventsManager, GetAnchor);
}

void RZUF3_WindowAnchor::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, WindowResize);
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, SetAnchor);
	_REMOVE_LISTENER(objEventsManager, GetAnchor);
}

void RZUF3_WindowAnchor::setAnchor(RZUF3_Anchor anchor)
{
	m_anchor = anchor;
	update();
}

void RZUF3_WindowAnchor::onWindowResize(RZUF3_WindowResizeEvent* event)
{
	m_width = event->GetWidth();
	m_height = event->GetHeight();

	update();
}

void RZUF3_WindowAnchor::onSetAnchor(RZUF3_SetAnchorEvent* event)
{
	setAnchor(event->getAnchor());
}

void RZUF3_WindowAnchor::onGetAnchor(RZUF3_GetAnchorEvent* event) const
{
	if (event->getAnchorIndex() == 0) {
		event->setAnchor(m_anchor);
	}
}

void RZUF3_WindowAnchor::update()
{
	int x = m_width * m_anchor.xAnchor + m_anchor.xOffset;
	int y = m_height * m_anchor.yAnchor + m_anchor.yOffset;

	RZUF3_Pos absPos = m_object->getAbsolutePos();
	RZUF3_Pos relPos = m_object->getPos();

	int relX = x - absPos.x + relPos.x;
	int relY = y - absPos.y + relPos.y;

	RZUF3_Pos newPos = {
		relX,
		relY,
		1,
		1
	};

	// spdlog::debug("WindowAnchor: {} {} {} {}", x, y, relX, relY);

	m_object->setPos(newPos);
}
