#pragma once
#include "game.h"
#include "scene_definition.h"
#include "scene.h"
#include "events/events.h"
#include "events_manager.h"

RZUF3_Game::RZUF3_Game()
{

}

RZUF3_Game::~RZUF3_Game()
{
	if(m_isRunning)
		clean();
}

void RZUF3_Game::initWindow(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		spdlog::error("SDL Initialization Failed: {}", SDL_GetError());
		return;
	}

	spdlog::info("SDL Initialized");

	int flags = 0;
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	m_window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
	if (!m_window)
	{
		spdlog::error("Window Creation Failed: {}", SDL_GetError());
		return;
	}

	spdlog::info("Window Created");

	m_sdlRenderer = SDL_CreateRenderer(m_window, -1, 0);
	if (!m_sdlRenderer)
	{
		spdlog::error("Renderer Creation Failed: {}", SDL_GetError());
		return;
	}

	m_renderer = new RZUF3_Renderer(m_sdlRenderer);

	spdlog::info("Renderer Created");

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		spdlog::error("IMG Initialization Failed: {}", IMG_GetError());
		return;
	}

	spdlog::info("IMG Initialized");

	int ttfFlags = TTF_Init();
	if (ttfFlags == -1)
	{
		spdlog::error("TTF Initialization Failed: {}", TTF_GetError());
		return;
	}

	spdlog::info("TTF Initialized");

	m_isRunning = true;
}

void RZUF3_Game::startGameLoop()
{
	uint64_t now = SDL_GetPerformanceCounter();
	uint64_t lastTime = 0;
	double deltaTime = 0;

	while (m_isRunning)
	{
		lastTime = now;
		now = SDL_GetPerformanceCounter();

		deltaTime = (double)((now - lastTime) * 1000 / (double)SDL_GetPerformanceFrequency());

		if(deltaTime < 1000.0f / RZUF3_GAME_FPS)
		{
			SDL_Delay((1000.0f / RZUF3_GAME_FPS) - deltaTime);
			deltaTime = 1000.0f / RZUF3_GAME_FPS;
		}

		handleSDLEvents();
		update(deltaTime);
		render(deltaTime);
	}
	clean();
}

void RZUF3_Game::handleSDLEvents()
{
	RZUF3_EventsManager* eventsManager = m_scene->getEventsManager();
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		RZUF3_Event* rEvent = nullptr;

		switch (sdlEvent.type)
		{
		case SDL_KEYDOWN:
			rEvent = new RZUF3_KeyDownEvent(sdlEvent.key.keysym.sym);
			break;
		case SDL_KEYUP:
			rEvent = new RZUF3_KeyUpEvent(sdlEvent.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			rEvent = new RZUF3_MouseDownEvent(sdlEvent.button.x, sdlEvent.button.y, sdlEvent.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			rEvent = new RZUF3_MouseUpEvent(sdlEvent.button.x, sdlEvent.button.y, sdlEvent.button.button);
			break;
		case SDL_MOUSEMOTION:
			rEvent = new RZUF3_MouseMoveEvent(sdlEvent.motion.x, sdlEvent.motion.y, sdlEvent.motion.xrel, sdlEvent.motion.yrel);
			break;
		case SDL_QUIT:
			m_isRunning = false;
			break;
		default:
			break;
		}

		if (rEvent != nullptr)
		{
			eventsManager->dispatchEvent(rEvent);
			delete rEvent;
		}
	}
}

void RZUF3_Game::clean()
{
	spdlog::info("Cleaning the game");
	delete m_scene;
	SDL_DestroyWindow(m_window);
	delete m_renderer;
	SDL_DestroyRenderer(m_sdlRenderer);
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}

void RZUF3_Game::setScene(RZUF3_SceneDefinition* sceneDefinition)
{
	if(this->m_scene != nullptr)
	{
		delete this->m_scene;
	}

	if(sceneDefinition == nullptr)
	{
		this->m_scene = nullptr;
		return;
	}

	this->m_scene = new RZUF3_Scene(sceneDefinition, this->m_renderer);
	this->m_scene->init();
}

RZUF3_Scene* RZUF3_Game::getScene()
{
	return this->m_scene;
}

void RZUF3_Game::update(double dt)
{
	if (m_scene != nullptr)
	{
		RZUF3_UpdateEvent rEvent(dt);
		m_scene->getEventsManager()->dispatchEvent((RZUF3_Event*)&rEvent);
	}

	m_counter++;
}

void RZUF3_Game::render(double dt)
{
	SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_sdlRenderer);
	if (m_scene != nullptr)
	{
		RZUF3_DrawEvent rEvent(dt);
		m_scene->getEventsManager()->dispatchEvent((RZUF3_Event*)&rEvent);
	}
	SDL_RenderPresent(m_sdlRenderer);
}