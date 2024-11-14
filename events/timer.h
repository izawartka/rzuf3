#pragma once
#include "events.h"

class RZUF3_TimerEvent : public RZUF3_Event {
public:
	RZUF3_TimerEvent(std::string id, double dt) {
		m_id = id;
		m_dt = dt;
	}

	std::string getId() const { return m_id; }
	double getDt() const { return m_dt; }

private:
	std::string m_id;
	double m_dt;
};
