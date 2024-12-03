#pragma once
#include "events.h"

class RZUF3_TextSelectionChangeEvent : public RZUF3_Event {
public:
	RZUF3_TextSelectionChangeEvent(int startPos, int endPos) {
		m_startPos = startPos;
		m_endPos = endPos;
	}

	int getStartPos() const { return m_startPos; }
	int getEndPos() const { return m_endPos; }

private:
	int m_startPos;
	int m_endPos;
};
