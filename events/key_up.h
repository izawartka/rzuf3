#pragma once
#include "events.h"

class RZUF3_KeyUpEvent : public RZUF3_Event {
public:
	RZUF3_KeyUpEvent(SDL_Keycode keyCode) {
		m_keyCode = keyCode;
	}

	SDL_Keycode getKeyCode() const { return m_keyCode; }

private:
	SDL_Keycode m_keyCode;
};