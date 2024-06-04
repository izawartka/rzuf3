#pragma once
#include "../common.h"
#include "../event.h"

#define RZUF3_EventType_RZUF 0
#define RZUF3_EventType_User 10000

enum RZUF3_EventType {
	RZUF3_EventType_Draw = RZUF3_EventType_RZUF,
	RZUF3_EventType_Update,
	RZUF3_EventType_KeyDown,
	RZUF3_EventType_KeyUp,
	RZUF3_EventType_MouseDown,
	RZUF3_EventType_MouseUp,
	RZUF3_EventType_MouseMove,
	RZUF3_EventType_MouseLeave,
	RZUF3_EventType_MouseEnter,
	RZUF3_EventType_MousePressed,
	RZUF3_EventType_UIValueChange,
	RZUF3_EventType_UISetValue,
};

#include "update.h"
#include "draw.h"
#include "key_down.h"
#include "key_up.h"
#include "mouse.h"
#include "mouse_down.h"
#include "mouse_up.h"
#include "mouse_move.h"
#include "mouse_leave.h"
#include "mouse_enter.h"
#include "mouse_pressed.h"
#include "ui_value_change.h"
#include "ui_set_value.h"