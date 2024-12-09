#pragma once
#include "events.h"

class RZUF3_DrawEvent : public RZUF3_Event {
public:
	RZUF3_DrawEvent(double dt) {
		m_dt = dt;
	}

	double getDt() const { return m_dt; }

private:
	double m_dt;
};