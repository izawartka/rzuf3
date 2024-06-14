#pragma once
#include "events.h"

class RZUF3_WindowResizeEvent : public RZUF3_Event {
public:
	RZUF3_WindowResizeEvent(int width, int height) {
		m_width = width;
		m_height = height;
	}

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	int m_width, m_height;
};