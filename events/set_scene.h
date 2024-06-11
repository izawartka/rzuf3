#pragma once
#include "events.h"
#include "../scene_definition.h"

struct RZUF3_SceneDefinition;

class RZUF3_SetSceneEvent : public RZUF3_Event {
public:
	RZUF3_SetSceneEvent(RZUF3_SceneDefinition* sceneDef) {
		m_sceneDef = sceneDef;
	}

	RZUF3_SceneDefinition* getSceneDefinition() { return m_sceneDef; }

private:
	RZUF3_SceneDefinition* m_sceneDef;
};