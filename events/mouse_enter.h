#pragma once
#include "events.h"

class RZUF3_MouseEnterEvent : public RZUF3_MouseEvent {
public:
	RZUF3_MouseEnterEvent(int x, int y)
		: RZUF3_MouseEvent(x, y) {}

	int getEventType() const { return RZUF3_EventType_MouseEnter; }
};