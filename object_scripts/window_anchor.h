#pragma once
#include "../event_macros.h"
#include "object_scripts.h"
#include "../anchor.h"

class RZUF3_ObjectScript;
class RZUF3_WindowResizeEvent;
class RZUF3_SetAnchorEvent;
class RZUF3_GetAnchorEvent;

/* currently doesn't work if the object is scaled */
class RZUF3_WindowAnchor : public RZUF3_ObjectScript {
public:
	RZUF3_WindowAnchor(double xAnchor, double yAnchor);
	RZUF3_WindowAnchor(RZUF3_Anchor anchor);
	~RZUF3_WindowAnchor();

	void init();
	void deinit();

	void setAnchor(RZUF3_Anchor anchor);

	RZUF3_Anchor getAnchor() const { return m_anchor; }

protected:
	void onWindowResize(RZUF3_WindowResizeEvent* event);
	void onSetAnchor(RZUF3_SetAnchorEvent* event);
	void onGetAnchor(RZUF3_GetAnchorEvent* event) const;

	void update();

	RZUF3_Anchor mp_anchor;

	RZUF3_Anchor m_anchor;
	int m_width = 0;
	int m_height = 0;

	_DECLARE_LISTENER(WindowResize)
	_DECLARE_LISTENER(SetAnchor)
	_DECLARE_LISTENER(GetAnchor)
};