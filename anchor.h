#pragma once
#include "common.h"

struct RZUF3_Anchor {
	double xAnchor = 0.5;
	double yAnchor = 0.5;
	int xOffset = 0;
	int yOffset = 0;

	RZUF3_Anchor() {}
	RZUF3_Anchor(double xAnchor, double yAnchor)
		: xAnchor(xAnchor), yAnchor(yAnchor) {}
	RZUF3_Anchor(double xAnchor, double yAnchor, int xOffset, int yOffset)
		: xAnchor(xAnchor), yAnchor(yAnchor), xOffset(xOffset), yOffset(yOffset) {}
};