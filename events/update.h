#pragma once
#include "events.h"

class RZUF3_UpdateEvent : public RZUF3_Event {
public:
	RZUF3_UpdateEvent(double dt) {
		this->m_dt = dt;
	}

	double getDt() const { return m_dt; }

private:
	double m_dt;
};