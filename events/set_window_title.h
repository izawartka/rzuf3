#pragma once
#include "events.h"

class RZUF3_SetWindowTitleEvent : public RZUF3_Event {
public:
	RZUF3_SetWindowTitleEvent(std::string title) {
		m_title = title;
	}

	std::string getTitle() const { return m_title; }

private:
	std::string m_title;
};