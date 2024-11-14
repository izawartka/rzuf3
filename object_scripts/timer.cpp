#include "timer.h"

RZUF3_Timer::RZUF3_Timer(std::string id, double interval)
{
	mp_options.id = id;
	mp_options.interval = interval;
}

RZUF3_Timer::RZUF3_Timer(RZUF3_TimerOptions options)
{
	mp_options = options;
}

RZUF3_Timer::~RZUF3_Timer()
{

}

void RZUF3_Timer::init()
{
	m_options = mp_options;

	m_selEventsManager = m_options.dispatchToScene ? g_scene->getEventsManager() : m_object->getEventsManager();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);
}

void RZUF3_Timer::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Update);

	m_selEventsManager = nullptr;
}

void RZUF3_Timer::setInterval(double interval)
{
	m_options.interval = interval;
}

void RZUF3_Timer::setEnabled(bool enabled)
{
	m_options.enabled = enabled;
}

void RZUF3_Timer::setTime(double time)
{
	m_time = time;
}

void RZUF3_Timer::onUpdate(RZUF3_UpdateEvent* event)
{
	if (!m_options.enabled) return;
	if (m_options.interval <= 0) return;
	if (m_selEventsManager == nullptr) return;

	double dt = event->getDt();
	m_time += dt;

	if (m_time < m_options.interval) return;

	RZUF3_TimerEvent* timerEvent = new RZUF3_TimerEvent(m_options.id, m_time);
	m_selEventsManager->dispatchEvent(timerEvent);
	// spdlog::debug("Timer event {} dispatched at time {}", m_options.id, m_time);
	m_time = 0;
}
