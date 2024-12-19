#pragma once
#include "common.h"

#define _ADD_LISTENER_NOID(eventsManager, eventType, libPrefix) \
	eventsManager->addEventListener<libPrefix##_##eventType##Event>([this](RZUF3_Event* event) { \
		on##eventType((libPrefix##_##eventType##Event*)event); \
	});

#define _ADD_LISTENER_MULTI(eventsManager, eventType, libPrefix, index) \
	m_id##eventType##Listener[index] = _ADD_LISTENER_NOID(eventsManager, eventType, libPrefix)

#define _ADD_LISTENER_CL(eventsManager, eventType, libPrefix) \
	m_id##eventType##Listener = _ADD_LISTENER_NOID(eventsManager, eventType, libPrefix)

#define _ADD_LISTENER(eventsManager, eventType) _ADD_LISTENER_CL(eventsManager, eventType, RZUF3)

#define _REMOVE_LISTENER_CL(eventsManager, eventType, libPrefix) \
	eventsManager->removeEventListener<libPrefix##_##eventType##Event>(m_id##eventType##Listener); \
	m_id##eventType##Listener = -1;

#define _REMOVE_LISTENER_MULTI(eventsManager, eventType, libPrefix, index) \
	eventsManager->removeEventListener<libPrefix##_##eventType##Event>(m_id##eventType##Listener[index]); \
	m_id##eventType##Listener.erase(index);

#define _REMOVE_LISTENER(eventsManager, eventType) _REMOVE_LISTENER_CL(eventsManager, eventType, RZUF3)

#define _DECLARE_LISTENER(eventType) \
	int m_id##eventType##Listener = -1;

#define _DECLARE_LISTENER_MULTI(eventType) \
	std::map<int, int> m_id##eventType##Listener;
