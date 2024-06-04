#pragma once
#include "events.h"

class RZUF3_UIValueChangeEvent : public RZUF3_Event {
public:
	RZUF3_UIValueChangeEvent(int value) {
		this->m_value = value;
	}

	int getEventType() const { return RZUF3_EventType_UIValueChange; }
	int getValue() const { return m_value; }

private:
	int m_value;
};