#pragma once
#include "events.h"

class RZUF3_UIValueChangeEvent : public RZUF3_Event {
public:
	RZUF3_UIValueChangeEvent(int value) {
		this->m_value = value;
	}

	int getValue() const { return m_value; }

private:
	int m_value;
};