#pragma once
#include "events.h"

class RZUF3_UISetStringValueEvent : public RZUF3_Event {
public:
	RZUF3_UISetStringValueEvent(std::string value) {
		m_value = value;
	}

	std::string getValue() const { return m_value; }

private:
	std::string m_value;
};