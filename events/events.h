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
	RZUF3_EventType_Quit,
	RZUF3_EventType_SetWindowSize,
	RZUF3_EventType_SetWindowIcon,
	RZUF3_EventType_SetWindowTitle,
	RZUF3_EventType_SetWindowFullscreen,
	RZUF3_EventType_SetScene,
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
#include "quit.h"
#include "set_window_size.h"
#include "set_window_icon.h"
#include "set_window_title.h"
#include "set_window_fullscreen.h"
#include "set_scene.h"