#pragma once
#include "events.h"

class RZUF3_TextEditingEvent : public RZUF3_Event {
public:
	RZUF3_TextEditingEvent(std::string text, int start, int length) {
		m_text = text;
		m_start = start;
		m_length = length;
	}

	std::string getText() const { return m_text; }
	int getStart() const { return m_start; }
	int getLength() const { return m_length; }

private:
	std::string m_text;
	int m_start;
	int m_length;
};
