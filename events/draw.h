#pragma once
#include "events.h"

class RZUF3_DrawEvent : public RZUF3_Event {
public:
	RZUF3_DrawEvent(double dt) {
		this->m_dt = dt;
	}

	int getEventType() const { return RZUF3_EventType_Draw; }
	double getDt() const { return m_dt; }

private:
	double m_dt;
};