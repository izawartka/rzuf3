#pragma once
#include "events.h"

class RZUF3_TextSelFocusEvent : public RZUF3_Event {
public:
	RZUF3_TextSelFocusEvent(std::string id) {
		m_id = id;
	}

	std::string getId() const { return m_id; }

private:
	std::string m_id;
};
