#pragma once
#include "events.h"

class RZUF3_TextInputFocusEvent : public RZUF3_Event {
public:
	RZUF3_TextInputFocusEvent(std::string id) {
		m_id = id;
	}

	std::string getId() const { return m_id; }

private:
	std::string m_id;
};
