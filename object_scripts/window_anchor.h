#pragma once
#include "../events/update.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_UpdateEvent;

struct RZUF3_WindowAnchorOptions {
	double xAnchor = 0.5;
	double yAnchor = 0.5;
	int xOffset = 0;
	int yOffset = 0;
};

/* currently doesn't work if the object is scaled */
class RZUF3_WindowAnchor : public RZUF3_ObjectScript {
public:
	RZUF3_WindowAnchor(double xAnchor, double yAnchor);
	RZUF3_WindowAnchor(RZUF3_WindowAnchorOptions options);
	~RZUF3_WindowAnchor();

	void init();
	void deinit();

	void setOptions(RZUF3_WindowAnchorOptions options);

	RZUF3_WindowAnchorOptions getOptions() const;

protected:
	void onWindowResize(RZUF3_WindowResizeEvent* event);
	void update();

	RZUF3_WindowAnchorOptions mp_options;

	RZUF3_WindowAnchorOptions m_options;
	int m_width = 0;
	int m_height = 0;

	_DECLARE_LISTENER(WindowResize)
};