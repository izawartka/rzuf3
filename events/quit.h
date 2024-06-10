#pragma once
#include "events.h"

class RZUF3_QuitEvent : public RZUF3_Event {
public:
	RZUF3_QuitEvent() {

	}

	int getEventType() const { return RZUF3_EventType_Quit; }
};