#pragma once
#include "events.h"
#include "../anchor.h"

class RZUF3_SetAnchorEvent : public RZUF3_Event {
public:
	RZUF3_SetAnchorEvent(RZUF3_Anchor anchor, int anchorIndex = 0) {
		m_anchor = anchor;
		m_anchorIndex = anchorIndex;
	}

	RZUF3_Anchor getAnchor() const {
		return m_anchor;
	}

	int getAnchorIndex() const {
		return m_anchorIndex;
	}

private:
	RZUF3_Anchor m_anchor;
	int m_anchorIndex;
};
