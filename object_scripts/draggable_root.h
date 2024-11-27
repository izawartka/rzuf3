#pragma once
#include "../events_manager.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_SetRectEvent;

struct RZUF3_DraggableRootOptions {
	std::string contentObjName;
};

class RZUF3_DraggableRoot : public RZUF3_ObjectScript {
public:
	RZUF3_DraggableRoot(RZUF3_DraggableRootOptions options);
	~RZUF3_DraggableRoot();

	void init();
	void deinit();

protected:
	void onDraggableUpdate(RZUF3_DraggableUpdateEvent* event);

	void linkContent();
	void updateContentPosition();

	RZUF3_DraggableRootOptions mp_options;

	int m_x = 0, m_y = 0;
	double m_scale = 1.0;
	RZUF3_Object* m_contentObj = nullptr;

	_DECLARE_LISTENER(DraggableUpdate)
};