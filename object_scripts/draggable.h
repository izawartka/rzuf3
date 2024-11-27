#pragma once
#include "../events_manager.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_SetRectEvent;

struct RZUF3_DraggableOptions {
	SDL_Rect rect = { 0, 0, 100, 100 };
	SDL_Rect limitRect = { 0, 0, 0, 0 };
	uint8_t dragButton = SDL_BUTTON_RIGHT;
	bool lockCursor = false;
	bool useDrag = true;
	bool useScale = true;
	double initialScale = 1.0;
	double minScale = 0.125;
	double maxScale = 4;
	double scaleStep = 0.125;
	bool updateOnInit = true;
	bool centerFix = true;
	bool scaleDragFix = false;
};

class RZUF3_Draggable : public RZUF3_ObjectScript {
public:
	RZUF3_Draggable(RZUF3_DraggableOptions options);
	~RZUF3_Draggable();

	void init();
	void deinit();

	void setRect(SDL_Rect rect);
	void setLimitRect(SDL_Rect limitRect);
	void forceUpdate();

protected:
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);
	void onMouseMove(RZUF3_MouseMoveEvent* event);
	void onMouseWheel(RZUF3_MouseWheelEvent* event);
	void onSetRect(RZUF3_SetRectEvent* event);

	void update();

	RZUF3_DraggableOptions mp_options;

	SDL_Rect m_rect = { 0, 0, 100, 100 };
	SDL_Rect m_limitRect = { 0, 0, 0, 0 };
	bool m_dragging = false;
	double m_x = 0.0;
	double m_y = 0.0;
	double m_scale = 1.0;
	int m_startCursorX = 0;
	int m_startCursorY = 0;

	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseUp)
	_DECLARE_LISTENER(MouseMove)
	_DECLARE_LISTENER(MouseWheel)
	_DECLARE_LISTENER(SetRect)
};