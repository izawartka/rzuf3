#pragma once
#include "events.h"

class RZUF3_MouseWheelEvent : public RZUF3_Event {
public:
	RZUF3_MouseWheelEvent(int x, int y)
		: RZUF3_Event(), m_x(x), m_y(y) {}

	int getX() const { return m_x; }
	int getY() const { return m_y; }

private:
	int m_x;
	int m_y;
};