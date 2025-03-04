#pragma once
#include "common.h"
#include "scene_definition.h"
#include "object_definition.h"
#include "assets_manager.h"
#include "object.h"
#include "event.h"
#include "event_listener.h"
#include "object_script.h"

struct RZUF3_SceneDefinition;
struct RZUF3_ObjectDefinition;
class RZUF3_AssetsManager;
class RZUF3_EventsManager;
class RZUF3_Object;
class RZUF3_ObjectScript;

class RZUF3_Scene {
public:
	RZUF3_Scene(RZUF3_SceneDefinition* sceneDefinition);
	~RZUF3_Scene();

	RZUF3_AssetsManager* getAssetsManager();
	RZUF3_EventsManager* getEventsManager();

	void init();

	std::string getName() const;
	RZUF3_Object* addObject(RZUF3_ObjectDefinition objectDef);
	void removeObject(std::string name);
	RZUF3_Object* getObject(std::string name);

private:
	std::string m_name;
	RZUF3_AssetsManager* m_assetsManager;
	RZUF3_EventsManager* m_eventsManager;
	std::map<std::string, RZUF3_Object*> m_objects;
	std::vector<RZUF3_Object*> m_objectsToDelete;
};

extern RZUF3_Scene* g_scene;