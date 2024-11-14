#pragma once
#include "events.h"

class RZUF3_TextInputEvent : public RZUF3_Event {
public:
	RZUF3_TextInputEvent(std::string text) {
		m_text = text;
	}

	std::string getText() const { return m_text; }

private:
	std::string m_text;
};
