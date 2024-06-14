#include "game.h"
#include "scene_definition.h"
#include "scene.h"
#include "events_manager.h"
#include "event_macros.h"

RZUF3_Game* g_game = nullptr;

RZUF3_Game::RZUF3_Game()
{
	assert(g_game == nullptr, "Only one instance of RZUF3_Game can be present at a time");
	g_game = this;
}

RZUF3_Game::~RZUF3_Game()
{
	if(m_isRunning)
		clean();

	g_game = nullptr;
}

void RZUF3_Game::loadLanguage(std::string filepath)
{
	if(m_lang != nullptr) delete m_lang;

	m_lang = new RZUF3_Lang(filepath);
	m_lang->load();
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
	delete m_lang;
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
	addGlobalEvents();
}

RZUF3_Scene* RZUF3_Game::getScene()
{
	return this->m_scene;
}

bool RZUF3_Game::setWindowIcon(std::string filepath)
{
	SDL_Surface* icon = IMG_Load(filepath.c_str());
	if (icon == nullptr)
	{
		spdlog::error("Failed to load icon: {}", IMG_GetError());
		return false;
	}

	SDL_SetWindowIcon(m_window, icon);
	SDL_FreeSurface(icon);
	return true;
}

void RZUF3_Game::addGlobalEvents()
{
	RZUF3_EventsManager* eventsManager = m_scene->getEventsManager();
	_ADD_LISTENER_NOID(eventsManager, Quit, RZUF3);
	_ADD_LISTENER_NOID(eventsManager, SetWindowSize, RZUF3);
	_ADD_LISTENER_NOID(eventsManager, SetWindowIcon, RZUF3);
	_ADD_LISTENER_NOID(eventsManager, SetWindowTitle, RZUF3);
	_ADD_LISTENER_NOID(eventsManager, SetWindowFullscreen, RZUF3);
	_ADD_LISTENER_NOID(eventsManager, SetScene, RZUF3);
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

void RZUF3_Game::onQuit(RZUF3_QuitEvent* event)
{
	m_isRunning = false;
}

void RZUF3_Game::onSetWindowSize(RZUF3_SetWindowSizeEvent* event)
{
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	if (event->getWidth() > 0) width = event->getWidth();
	if (event->getHeight() > 0) height = event->getHeight();
	SDL_SetWindowSize(m_window, width, height);
}

void RZUF3_Game::onSetWindowIcon(RZUF3_SetWindowIconEvent* event)
{
	setWindowIcon(event->getFilename());
}

void RZUF3_Game::onSetWindowTitle(RZUF3_SetWindowTitleEvent* event)
{
	SDL_SetWindowTitle(m_window, event->getTitle().c_str());
}

void RZUF3_Game::onSetWindowFullscreen(RZUF3_SetWindowFullscreenEvent* event)
{
	SDL_SetWindowFullscreen(m_window, event->getFlags());
}

void RZUF3_Game::onSetScene(RZUF3_SetSceneEvent* event)
{
	setScene(event->getSceneDefinition());
}
