#include "scene.h"
#include "assets_manager.h"
#include "events_manager.h"
#include "scene_definition.h"
#include "object_definition.h"
#include "object.h"
#include "game.h"

RZUF3_Scene* g_scene = nullptr;

RZUF3_Scene::RZUF3_Scene(RZUF3_SceneDefinition* sceneDefinition)
{
	if(g_scene != nullptr) throw std::logic_error("Only one instance of RZUF3_Scene can be present at a time");
	g_scene = this;

	m_name = sceneDefinition->name;
	m_assetsManager = new RZUF3_AssetsManager();
	m_assetsManager->addAssets(sceneDefinition->staticAssets);
	m_eventsManager = new RZUF3_EventsManager();

	for (auto& objectDefinition : sceneDefinition->objects)
	{
		RZUF3_Object* object = addObject(objectDefinition);
		m_objectsToDelete.push_back(object);
	}
}

RZUF3_Scene::~RZUF3_Scene()
{
	for (auto& object : m_objectsToDelete)
	{
#ifdef _DEBUG
		std::string name = object->getName();
		delete object;
		m_objects.erase(name);
#else
		delete object;
#endif // _DEBUG
	}

#ifdef _DEBUG
	for (auto& object : m_objects)
	{
		spdlog::warn("Object {} was not removed before scene destruction", object.first);
	}
#endif // _DEBUG


	delete m_assetsManager;
	m_eventsManager->destroy();

	g_scene = nullptr;
}

RZUF3_AssetsManager* RZUF3_Scene::getAssetsManager()
{
	return m_assetsManager;
}

RZUF3_EventsManager* RZUF3_Scene::getEventsManager()
{
	return m_eventsManager;
}

void RZUF3_Scene::init()
{
	for (auto& object : m_objects)
	{
		object.second->init();
	}
}

std::string RZUF3_Scene::getName() const
{
	return m_name;
}

RZUF3_Object* RZUF3_Scene::addObject(RZUF3_ObjectDefinition objectDef)
{
	if(m_objects.find(objectDef.name) != m_objects.end())
	{
		spdlog::error("Object with name {} already exists in the scene", objectDef.name);
		return nullptr;
	}

	RZUF3_Object* object = new RZUF3_Object(objectDef, this);
	m_objects.insert(std::pair(objectDef.name, object));

	return object;
}

void RZUF3_Scene::removeObject(std::string name)
{
	RZUF3_Object* object = getObject(name);
	if (object == nullptr) return;

	delete m_objects[name];
	m_objects.erase(name);
}

RZUF3_Object* RZUF3_Scene::getObject(std::string name)
{
	if (m_objects.find(name) == m_objects.end())
	{
		spdlog::warn("Object with name {} does not exist in the scene", name);
		return nullptr;
	}

	return m_objects[name];
}
