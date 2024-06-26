#pragma once
#include "common.h"
#include "event.h"

typedef std::function<void(RZUF3_Event*)> RZUF3_EventCallback;

struct RZUF3_EventListener {
public:
	int id;
	std::type_index eventType;
	RZUF3_EventCallback callback;

	RZUF3_EventListener(
		int id,
		std::type_index eventType,
		RZUF3_EventCallback callback
	) : id(id), eventType(eventType), callback(callback) {}
};