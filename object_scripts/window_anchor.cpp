#include "window_anchor.h"
#include "../game.h"

RZUF3_WindowAnchor::RZUF3_WindowAnchor(double xAnchor, double yAnchor)
{
	mp_options.xAnchor = xAnchor;
	mp_options.yAnchor = yAnchor;
}

RZUF3_WindowAnchor::RZUF3_WindowAnchor(RZUF3_WindowAnchorOptions options)
{
	mp_options = options;
}

RZUF3_WindowAnchor::~RZUF3_WindowAnchor()
{

}

void RZUF3_WindowAnchor::init()
{
	m_options = mp_options;

	g_game->getWindowSize(&m_width, &m_height);
	update();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, WindowResize);
}

void RZUF3_WindowAnchor::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, WindowResize);
}

void RZUF3_WindowAnchor::setOptions(RZUF3_WindowAnchorOptions options)
{
	m_options = options;
	update();
}

RZUF3_WindowAnchorOptions RZUF3_WindowAnchor::getOptions() const
{
	return m_options;
}

void RZUF3_WindowAnchor::onWindowResize(RZUF3_WindowResizeEvent* event)
{
	m_width = event->GetWidth();
	m_height = event->GetHeight();

	update();
}

void RZUF3_WindowAnchor::update()
{
	int x = m_width * m_options.xAnchor + m_options.xOffset;
	int y = m_height * m_options.yAnchor + m_options.yOffset;

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
