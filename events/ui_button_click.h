#pragma once
#include "events.h"

class RZUF3_UIButtonClickEvent : public RZUF3_Event {
public:
	RZUF3_UIButtonClickEvent(std::string id)
		: RZUF3_Event(), m_id(id) {}

	std::string getID() const { return m_id; }

private:
	std::string m_id;
};