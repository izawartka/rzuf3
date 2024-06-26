#pragma once
#include "../events/update.h"
#include "../event_macros.h"
#include "../anchor.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_UpdateEvent;
class RZUF3_WindowResizeEvent;
class RZUF3_SetAnchorEvent;
class RZUF3_GetAnchorEvent;

struct RZUF3_WindowRectAnchorOptions {
	RZUF3_Anchor anchors[2];
};

/* currently doesn't work if the parent object is moved or scaled */
class RZUF3_WindowRectAnchor : public RZUF3_ObjectScript {
public:
	RZUF3_WindowRectAnchor(RZUF3_WindowRectAnchorOptions options);
	~RZUF3_WindowRectAnchor();

	void init();
	void deinit();

	void setOptions(RZUF3_WindowRectAnchorOptions options);
	void setAnchor(RZUF3_Anchor anchor, int index);

	RZUF3_WindowRectAnchorOptions getOptions();

protected:
	void onUpdate(RZUF3_UpdateEvent* event);
	void onWindowResize(RZUF3_WindowResizeEvent* event);
	void onSetAnchor(RZUF3_SetAnchorEvent* event);
	void onGetAnchor(RZUF3_GetAnchorEvent* event) const;
	void update();

	RZUF3_WindowRectAnchorOptions mp_options;

	RZUF3_EventsManager* m_objEventsManager = nullptr;
	RZUF3_WindowRectAnchorOptions m_options;
	int m_width = 0;
	int m_height = 0;
	bool m_isFirstUpdate = true;

	_DECLARE_LISTENER(Update)
	_DECLARE_LISTENER(WindowResize)
	_DECLARE_LISTENER(SetAnchor)
	_DECLARE_LISTENER(GetAnchor)
};