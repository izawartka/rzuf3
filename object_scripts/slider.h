#pragma once
#include "../events/draw.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_MousePressedEvent;
class RZUF3_DrawEvent;

struct RZUF3_SliderStyle {
	SDL_Color sliderColor = { 128, 128, 128, 255 };
	SDL_Color thumbColor = { 255, 255, 255, 255 };
	int sliderSize = 4;
	int thumbSize = 14;
	int width = 48;
};

struct RZUF3_SliderOptions {
	int min;
	int max;
	int value;
	RZUF3_SliderStyle style;
};

class RZUF3_Slider : public RZUF3_ObjectScript {
public:
	RZUF3_Slider(int min, int max, int value);
	RZUF3_Slider(RZUF3_SliderOptions options);
	~RZUF3_Slider();

	void init();
	void deinit();

	void setStyle(RZUF3_SliderStyle style);
	void setMin(int min);
	void setMax(int max);
	void setValue(int value);

	RZUF3_SliderStyle getStyle() const;
	int getValue() const;

protected:
	void onMousePressed(RZUF3_MousePressedEvent* event);
	void onUISetValue(RZUF3_UISetValueEvent* event);
	void onDraw(RZUF3_DrawEvent* event);

	void updateStyle();

	RZUF3_SliderOptions mp_options;

	int m_min = 0;
	int m_max = 0;
	int m_value = 0;
	RZUF3_SliderStyle m_style;
	RZUF3_EventsManager* m_objEventsManager = nullptr;
	RZUF3_Clickable* m_clickable = nullptr;

	_DECLARE_LISTENER(MousePressed)
	_DECLARE_LISTENER(UISetValue)
	_DECLARE_LISTENER(Draw)
};