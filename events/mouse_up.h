#pragma once
#include "events.h"

class RZUF3_MouseUpEvent : public RZUF3_MouseEvent {
public:
	RZUF3_MouseUpEvent(int x, int y, uint8_t button)
		: RZUF3_MouseEvent(x, y), m_button(button) {}

	int getEventType() const { return RZUF3_EventType_MouseUp; }
	uint8_t getButton() const { return m_button; }

protected:
	uint8_t m_button;
};