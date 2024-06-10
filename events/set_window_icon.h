#pragma once
#include "events.h"

class RZUF3_SetWindowIconEvent : public RZUF3_Event {
public:
	RZUF3_SetWindowIconEvent(std::string filename) {
		m_filename = filename;
	}

	int getEventType() const { return RZUF3_EventType_SetWindowIcon; }
	std::string getFilename() const { return m_filename; }

private:
	std::string m_filename;
};