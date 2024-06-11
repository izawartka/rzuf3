#pragma once
#include "events.h"

class RZUF3_UISetValueEvent : public RZUF3_Event {
public:
	RZUF3_UISetValueEvent(int value) {
		this->m_value = value;
	}

	int getValue() const { return m_value; }

private:
	int m_value;
};