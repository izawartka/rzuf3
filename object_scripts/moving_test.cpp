#include "moving_test.h"
#include "../events_manager.h"

RZUF3_MovingTest::RZUF3_MovingTest()
{

}

RZUF3_MovingTest::~RZUF3_MovingTest()
{

}

void RZUF3_MovingTest::init()
{	
	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);
}

void RZUF3_MovingTest::deinit()
{
	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Update);

	m_angle = 0;
}

void RZUF3_MovingTest::onUpdate(RZUF3_UpdateEvent* event)
{
	m_angle += 0.001 * event->getDt();

	int x = 200 + 100 * sin(m_angle);
	int y = 200 + 100 * cos(m_angle);

	RZUF3_Pos pos(x, y);
	m_object->setPos(pos);
}
