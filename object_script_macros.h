#pragma once
#include "common.h"

#define _ADD_LISTENER_CL(eventsManager, eventType, libPrefix) \
	m_id##eventType##Listener = eventsManager->addEventListener(##libPrefix##_EventType_##eventType##, [this](RZUF3_Event* event) { \
		on##eventType##((##libPrefix##_##eventType##Event*)event); \
	});

#define _ADD_LISTENER(eventsManager, eventType) _ADD_LISTENER_CL(eventsManager, eventType, RZUF3)

#define _REMOVE_LISTENER_CL(eventsManager, eventType, libPrefix) \
	eventsManager->removeEventListener(##libPrefix##_EventType_##eventType##, m_id##eventType##Listener); \
	m_id##eventType##Listener = -1;

#define _REMOVE_LISTENER(eventsManager, eventType) _REMOVE_LISTENER_CL(eventsManager, eventType, RZUF3)

#define _DECLARE_LISTENER(eventType) \
	int m_id##eventType##Listener = -1;

#define _ADD_BASIC_PARAM_SET(name) \
	if (!m_object) { \
		mp_##name## = ##name##; \
		return this; \
	}

#define _ADD_BASIC_PARAM_GET(name) \
	if (!m_object) { \
		return mp_##name##; \
	}