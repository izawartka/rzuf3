#pragma once
#include "../events/draw.h"
#include "../event_macros.h"
#include "../renderer.h"
#include "object_scripts.h"
#include "border_box_style.h"

class RZUF3_ObjectScript;
class RZUF3_Renderer;
class RZUF3_Clickable;
class RZUF3_TextRenderer;
class RZUF3_DrawEvent;
class RZUF3_MouseDownEvent;
class RZUF3_UISetValueEvent;
class RZUF3_DrawEvent;

struct RZUF3_BorderBoxOptions {
	RZUF3_BorderBoxStyle style;
	bool useOnSetRect = true;
	bool useOnDraw = true;
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
	SDL_Rect rect;
};

class RZUF3_BorderBox : public RZUF3_ObjectScript {
public:
	RZUF3_BorderBox(RZUF3_BorderBoxOptions options);
	~RZUF3_BorderBox();

	void init();
	void deinit();

	void setStyle(RZUF3_BorderBoxStyle style);
	void setUseOnSetRect(bool useOnSetRect);
	void setUseOnDraw(bool useOnDraw);
	void setAlignment(RZUF3_Align alignment);
	void setRect(SDL_Rect rect);

	void draw();

	RZUF3_BorderBoxStyle getStyle() const;
	bool getUseOnSetRect() const;
	bool getUseOnDraw() const;
	RZUF3_Align getAlignment() const;
	SDL_Rect getRect() const;

protected:
	void onDraw(RZUF3_DrawEvent* event);
	void onSetRect(RZUF3_SetRectEvent* event);

	RZUF3_BorderBoxOptions mp_options;

	RZUF3_BorderBoxOptions m_options;
	bool m_hasOnDrawListener = false;
	bool m_hasOnSetRectListener = false;

	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(SetRect)
};