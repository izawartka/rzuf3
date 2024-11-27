#pragma once
#include "events.h"
#include "../rectd.h"

class RZUF3_DraggableUpdateEvent : public RZUF3_Event {
public:
	RZUF3_DraggableUpdateEvent(double x, double y, double scale) {
		m_x = x;
		m_y = y;
		m_scale = scale;
	}

	double getX() const { return m_x; }
	double getY() const { return m_y; }
	double getScale() const { return m_scale; }
	RZUF3_RectD getAsRectD() const { return { m_x, m_y, m_scale, m_scale }; }

private:
	double m_x;
	double m_y;
	double m_scale;
};