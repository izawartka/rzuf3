#pragma once
#include "events.h"

class RZUF3_MouseLeaveEvent : public RZUF3_MouseEvent {
public:
	RZUF3_MouseLeaveEvent(int x, int y)
		: RZUF3_MouseEvent(x, y) {}
};