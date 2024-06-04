#pragma once
#include "events.h"

class RZUF3_MouseEvent : public RZUF3_Event {
public:
	int getX() const { return m_x; }
	int getY() const { return m_y; }

protected:
	RZUF3_MouseEvent(int x, int y) 
		: m_x(x), m_y(y) {}

	int m_x;
	int m_y;
};