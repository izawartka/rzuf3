#pragma once
#include "events.h"

class RZUF3_MouseMoveEvent : public RZUF3_MouseEvent {
public:
	RZUF3_MouseMoveEvent(int x, int y, int movX, int movY)
		: RZUF3_MouseEvent(x, y), m_movX(movX), m_movY(movY) {}

	int getMovX() const { return m_movX; }
	int getMovY() const { return m_movY; }

protected:
	int m_movX, m_movY;
};