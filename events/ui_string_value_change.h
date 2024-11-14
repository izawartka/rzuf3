#pragma once
#include "events.h"

class RZUF3_UIStringValueChangeEvent : public RZUF3_Event {
public:
	RZUF3_UIStringValueChangeEvent(std::string value) {
		this->m_value = value;
	}

	std::string getValue() const { return m_value; }

private:
	std::string m_value;
};