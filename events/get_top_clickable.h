#pragma once
#include "events.h"

class RZUF3_Clickable;

class RZUF3_GetTopClickableEvent : public RZUF3_MouseEvent {
public:
	RZUF3_GetTopClickableEvent(int x, int y)
		: RZUF3_MouseEvent(x, y) {}

	void respond(RZUF3_Clickable* clickable) {
		m_topClickable = clickable;
	}

	RZUF3_Clickable* getTopClickable() {
		return m_topClickable;
	}

protected:
	RZUF3_Clickable* m_topClickable = nullptr;
};