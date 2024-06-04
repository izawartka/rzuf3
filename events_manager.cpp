#include "events_manager.h"

RZUF3_EventsManager::RZUF3_EventsManager()
{

}

RZUF3_EventsManager::~RZUF3_EventsManager()
{

}


int RZUF3_EventsManager::addEventListener(int eventType, RZUF3_EventCallback callback)
{
	int id = m_nextEventListenerID++;

	RZUF3_EventListener eventListener(
		id,
		eventType,
		callback
	);

	auto it = m_eventListeners.find(eventType);

	if (it == m_eventListeners.end())
	{
		std::vector<RZUF3_EventListener> typeEventListeners;
		typeEventListeners.push_back(eventListener);
		std::pair<int, std::vector<RZUF3_EventListener>> pair(eventType, typeEventListeners);
		m_eventListeners.insert(pair);
	}
	else
	{
		it->second.push_back(eventListener);
	}

	return id;
}

void RZUF3_EventsManager::removeEventListener(int eventType, int eventListenerID)
{
	auto it = m_eventListeners.find(eventType);

	if (it == m_eventListeners.end())
	{
		return;
	}

	it->second.erase(
		std::remove_if(
			it->second.begin(),
			it->second.end(),
			[eventListenerID](RZUF3_EventListener& eventListener) { return eventListener.id == eventListenerID; }
		),
		it->second.end()
	);
}

void RZUF3_EventsManager::dispatchEvent(RZUF3_Event* event)
{
	auto it = m_eventListeners.find(event->getEventType());

	if (it == m_eventListeners.end())
	{
		return;
	}

	for (RZUF3_EventListener& eventListener : it->second)
	{
		eventListener.callback(event);
	}
}
