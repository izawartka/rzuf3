#pragma once
#include "events.h"

class RZUF3_SetWindowFullscreenEvent : public RZUF3_Event {
public:
	RZUF3_SetWindowFullscreenEvent(Uint32 flags) {
		m_flags = flags;
	}

	int getEventType() const { return RZUF3_EventType_SetWindowFullscreen; }
	Uint32 getFlags() const { return m_flags; }

private:
	Uint32 m_flags;
};