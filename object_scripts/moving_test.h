#pragma once
#include "../events/update.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_UpdateEvent;

class RZUF3_MovingTest : public RZUF3_ObjectScript {
public:
	RZUF3_MovingTest();
	~RZUF3_MovingTest();

	void init();
	void deinit();

protected:
	void onUpdate(RZUF3_UpdateEvent* event);

	double m_angle = 0;

	_DECLARE_LISTENER(Update)
};