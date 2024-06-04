#pragma once
#include "common.h"

class RZUF3_Event {
public:
	virtual int getEventType() const { return -1; }
};