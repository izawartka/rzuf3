#pragma once
#include "events.h"
#include "../anchor.h"

class RZUF3_GetAnchorEvent : public RZUF3_Event {
public:
	RZUF3_GetAnchorEvent(int anchorIndex = 0) {
		m_anchorIndex = anchorIndex;
	}

	void setAnchor(RZUF3_Anchor anchor) {
		if (m_reachedTarget) {
			spdlog::error("RZUF3_GetAnchorEvent reached multiple targets");
		}

		m_anchor = anchor;
		m_reachedTarget = true;
	}

	RZUF3_Anchor getAnchor() const {
		if (!m_reachedTarget) {
			spdlog::error("RZUF3_GetAnchorEvent target not reached");
		}

		return m_anchor;
	}

	int getAnchorIndex() const {
		return m_anchorIndex;
	}

	bool reachedTarget() const {
		return m_reachedTarget;
	}

private:
	RZUF3_Anchor m_anchor;
	int m_anchorIndex = 0;
	bool m_reachedTarget = false;
};
