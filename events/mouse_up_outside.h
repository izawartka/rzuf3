#pragma once
#include "events.h"

class RZUF3_MouseUpOutsideEvent : public RZUF3_MouseEvent {
public:
	RZUF3_MouseUpOutsideEvent(int x, int y, uint8_t button)
		: RZUF3_MouseEvent(x, y), m_button(button) {}

	uint8_t getButton() const { return m_button; }

protected:
	uint8_t m_button;
};