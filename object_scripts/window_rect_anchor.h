#pragma once
#include "../events/update.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_UpdateEvent;

struct RZUF3_WindowRectAnchorOptions {
	double x1Anchor = 0;
	double y1Anchor = 0;
	double x2Anchor = 1;
	double y2Anchor = 1;
	int x1Offset = 0;
	int y1Offset = 0;
	int x2Offset = 0;
	int y2Offset = 0;
};

/* currently doesn't work if the parent object is moved or scaled */
class RZUF3_WindowRectAnchor : public RZUF3_ObjectScript {
public:
	RZUF3_WindowRectAnchor(RZUF3_WindowRectAnchorOptions options);
	~RZUF3_WindowRectAnchor();

	void init();
	void deinit();

	void setOptions(RZUF3_WindowRectAnchorOptions options);

	RZUF3_WindowRectAnchorOptions getOptions();

protected:
	void onUpdate(RZUF3_UpdateEvent* event);
	void onWindowResize(RZUF3_WindowResizeEvent* event);
	void update();

	RZUF3_WindowRectAnchorOptions mp_options;

	RZUF3_EventsManager* m_objEventsManager = nullptr;
	RZUF3_WindowRectAnchorOptions m_options;
	int m_width = 0;
	int m_height = 0;
	bool m_isFirstUpdate = true;

	_DECLARE_LISTENER(Update)
	_DECLARE_LISTENER(WindowResize)
};