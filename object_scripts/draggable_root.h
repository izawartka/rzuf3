#pragma once
#include "../events_manager.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

struct RZUF3_DraggableRootOptions {
	std::string contentObjName;
	SDL_Rect rect = { 0, 0, 100, 100 };
	SDL_Rect limitRect = { 0, 0, 0, 0 };
	uint8_t dragButton = SDL_BUTTON_RIGHT;
	bool lockCursor = false;
};

class RZUF3_DraggableRoot : public RZUF3_ObjectScript {
public:
	RZUF3_DraggableRoot(RZUF3_DraggableRootOptions options);
	~RZUF3_DraggableRoot();

	void setRect(SDL_Rect rect);
	void setLimitRect(SDL_Rect limitRect);

	void init();
	void deinit();

protected:
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);
	void onMouseMove(RZUF3_MouseMoveEvent* event);
	void onSetRect(RZUF3_SetRectEvent* event);

	void linkContent();
	void updateContentPosition();

	RZUF3_DraggableRootOptions mp_options;

	RZUF3_Object* m_contentObj = nullptr;
	SDL_Rect m_rect = { 0, 0, 100, 100 };
	SDL_Rect m_limitRect = { 0, 0, 0, 0 };
	bool m_dragging = false;
	int m_offX = 0;
	int m_offY = 0;
	int m_startCursorX = 0;
	int m_startCursorY = 0;

	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseUp)
	_DECLARE_LISTENER(MouseMove)
	_DECLARE_LISTENER(SetRect)
};