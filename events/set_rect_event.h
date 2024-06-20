#pragma once
#include "events.h"

class RZUF3_SetRectEvent : public RZUF3_Event {
public:
	RZUF3_SetRectEvent(SDL_Rect rect) {
		m_rect = rect;
	}

	SDL_Rect getRect() const {
		return m_rect;
	}

private:
	SDL_Rect m_rect;
};
