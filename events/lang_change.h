#pragma once
#include "events.h"

class RZUF3_Lang;

class RZUF3_LangChangeEvent : public RZUF3_Event {
public:
	RZUF3_LangChangeEvent(RZUF3_Lang* newLang) {
		m_newLang = newLang;
	}

	RZUF3_Lang* getNewLang() { return m_newLang; }

private:
	RZUF3_Lang* m_newLang;
};