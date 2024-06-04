#pragma once
#include "common.h"
#include "event.h"
#include "event_listener.h"

class RZUF3_Event;
struct RZUF3_EventListener;

class RZUF3_EventsManager {
public:
	RZUF3_EventsManager();
	~RZUF3_EventsManager();

	int addEventListener(int eventType, RZUF3_EventCallback callback);
	void removeEventListener(int eventType, int eventListenerID);
	void dispatchEvent(RZUF3_Event* event);

private:
	std::map<int, std::vector<RZUF3_EventListener>> m_eventListeners;
	int m_nextEventListenerID = 0;
};