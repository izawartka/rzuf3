#pragma once
#include "events.h"

class RZUF3_KeyDownEvent : public RZUF3_Event {
public:
	RZUF3_KeyDownEvent(SDL_Keycode keyCode) {
		this->m_keyCode = keyCode;
	}

	int getEventType() const { return RZUF3_EventType_KeyDown; }
	SDL_Keycode getKeyCode() const { return m_keyCode; }

private:
	SDL_Keycode m_keyCode;
};