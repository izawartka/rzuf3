#include "scene.h"
#include "assets_manager.h"
#include "events_manager.h"
#include "scene_definition.h"
#include "object_definition.h"
#include "object.h"

RZUF3_Scene::RZUF3_Scene(RZUF3_SceneDefinition* sceneDefinition, RZUF3_Renderer* renderer)
{
	this->m_name = sceneDefinition->name;
	this->m_renderer = renderer;
	this->m_assetsManager = new RZUF3_AssetsManager(renderer);
	this->m_assetsManager->addAssets(sceneDefinition->staticAssets);
	this->m_eventsManager = new RZUF3_EventsManager();

	for (auto& objectDefinition : sceneDefinition->objects)
	{
		this->addObject(objectDefinition);
	}
}

RZUF3_Scene::~RZUF3_Scene()
{
	for (auto& object : this->m_objects)
	{
		delete object.second;
	}

	delete this->m_assetsManager;
	delete this->m_eventsManager;
}

RZUF3_AssetsManager* RZUF3_Scene::getAssetsManager()
{
	return this->m_assetsManager;
}

RZUF3_EventsManager* RZUF3_Scene::getEventsManager()
{
	return this->m_eventsManager;
}

RZUF3_Renderer* RZUF3_Scene::getRenderer()
{
	return this->m_renderer;
}

void RZUF3_Scene::init()
{
	for (auto& object : this->m_objects)
	{
		object.second->init();
	}
}

bool RZUF3_Scene::addObject(RZUF3_ObjectDefinition objectDef)
{
	if(this->m_objects.find(objectDef.name) != this->m_objects.end())
	{
		spdlog::error("Object with name {} already exists in scene {}", objectDef.name, this->m_name);
		return false;
	}

	RZUF3_Object* object = new RZUF3_Object(objectDef, this);
	this->m_objects.insert(std::pair<std::string, RZUF3_Object*>(objectDef.name, object));
	return true;
}

void RZUF3_Scene::removeObject(std::string name)
{
	if(this->m_objects.find(name) == this->m_objects.end())
	{
		spdlog::warn("Object with name {} does not exist in scene {}", name, this->m_name);
		return;
	}

	delete this->m_objects[name];
	this->m_objects.erase(name);
}

RZUF3_Object* RZUF3_Scene::getObject(std::string name)
{
	return this->m_objects[name];
}