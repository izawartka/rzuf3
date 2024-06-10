#pragma once
#include "events.h"

class RZUF3_SetWindowSizeEvent : public RZUF3_Event {
public:
	RZUF3_SetWindowSizeEvent(int width, int height) {
		m_width = width;
		m_height = height;
	}

	int getEventType() const { return RZUF3_EventType_SetWindowSize; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }

private:
	int m_width;
	int m_height;
};