#include "common.h"
#include "game.h"
#include "scene_definition.h"
#include "scene.h"
#include "events_manager.h"
#include "event_macros.h"
#include "config_file.h"

RZUF3_Game* g_game = nullptr;

RZUF3_Game::RZUF3_Game()
{
	if (g_game != nullptr) throw std::logic_error("Only one instance of RZUF3_Game can be present at a time");
	g_game = this;
}

RZUF3_Game::~RZUF3_Game()
{
	if(m_isRunning)
		clean();

	g_game = nullptr;
}

bool RZUF3_Game::addConfigFile(RZUF3_ConfigFileDef def)
{
	auto it = m_configFiles.find(def.filepath);
	if (it != m_configFiles.end())
	{
		spdlog::warn("Config file {} is already loaded", def.filepath);
		return false;
	}

	RZUF3_ConfigFile* configFile = new RZUF3_ConfigFile(def);

	m_configFiles[def.filepath] = configFile;

	if (def.isDefaultConfig) {
		if (m_defaultConfigFile != nullptr) {
			spdlog::warn("Trying to set {} as default config file, but {} is already set", def.filepath, m_defaultConfigFile->getFilepath());
			return false;
		}

		m_defaultConfigFile = configFile;
	}

	return true;
}

void RZUF3_Game::loadLanguage(std::string filepath)
{
	if(m_lang != nullptr) delete m_lang;

	m_lang = new RZUF3_Lang(filepath);
	m_lang->load();
}

void RZUF3_Game::loadLanguageFromConfigFile(std::string basepath, std::string valueKey)
{
	if(m_defaultConfigFile == nullptr)
	{
		spdlog::error("Cannot load language file from config file, no default config file set");
		return;
	}

	std::string filename = m_defaultConfigFile->getValue(valueKey);
	
	if(filename == "")
	{
		spdlog::error("Cannot load language file from config file, key {} not found", valueKey);
		return;
	}

	if(filename.find("..") != std::string::npos || filename.find("/") != std::string::npos || filename.find("\\") != std::string::npos)
	{
		spdlog::error("Cannot load language file from config file, invalid filename");
		return;
	}

	std::string filepath = basepath + filename;

	loadLanguage(filepath);
}

void RZUF3_Game::initWindow(int width, int height, bool fullscreen)
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

	m_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
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
	setWindowSize(width, height);

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

		if(RZUF3_GAME_FPS > 0 && deltaTime < 1000.0f / RZUF3_GAME_FPS)
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

void RZUF3_Game::quit()
{
	m_isRunning = false;
}

void RZUF3_Game::setWindowTitle(std::string title, bool useLangFile)
{
	if(useLangFile && m_lang != nullptr) title = m_lang->getText(title);
	SDL_SetWindowTitle(m_window, title.c_str());
}

void RZUF3_Game::setWindowSize(int width, int height)
{
	int oldWidth, oldHeight;
	SDL_GetWindowSize(m_window, &oldWidth, &oldHeight);
	if (width <= 0) width = oldWidth;
	if (height <= 0) height = oldHeight;
	SDL_SetWindowSize(m_window, width, height);
}

void RZUF3_Game::setWindowFullscreen(bool fullscreen)
{
	SDL_SetWindowFullscreen(m_window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
}

void RZUF3_Game::setWindowResizable(bool resizable)
{
	SDL_SetWindowResizable(m_window, resizable ? SDL_TRUE : SDL_FALSE);
}

void RZUF3_Game::setWindowMinimumSize(int width, int height)
{
	int oldWidth, oldHeight;
	SDL_GetWindowMinimumSize(m_window, &oldWidth, &oldHeight);
	if (width <= 0) width = oldWidth;
	if (height <= 0) height = oldHeight;
	SDL_SetWindowMinimumSize(m_window, width, height);
}

void RZUF3_Game::setWindowMaximumSize(int width, int height)
{
	int oldWidth, oldHeight;
	SDL_GetWindowMaximumSize(m_window, &oldWidth, &oldHeight);
	if (width <= 0) width = oldWidth;
	if (height <= 0) height = oldHeight;
	SDL_SetWindowMaximumSize(m_window, width, height);
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

void RZUF3_Game::setRelativeMouseMode(bool enabled)
{
	SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
}

void RZUF3_Game::setCursorPos(int x, int y)
{
	SDL_WarpMouseInWindow(m_window, x, y);
}

void RZUF3_Game::setCursorVisible(bool visible)
{
	SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

void RZUF3_Game::requestTextInput(std::string id, SDL_Rect rect)
{
	if(m_scene == nullptr) return;
	if(id == "") return;

	if (m_currentTextInputId != "")
	{
		stopTextInput(m_currentTextInputId);
	}

	m_currentTextInputId = id;

	RZUF3_TextInputFocusEvent event(id);
	m_scene->getEventsManager()->dispatchEvent(&event);

	SDL_SetTextInputRect(&rect);
	SDL_StartTextInput();
}

void RZUF3_Game::stopTextInput(std::string id)
{
	if(m_scene == nullptr) return;
	if(m_currentTextInputId != id && id != "") return;

	m_currentTextInputId = "";

	SDL_StopTextInput();

	RZUF3_TextInputFocusEvent event("");
	m_scene->getEventsManager()->dispatchEvent(&event);
}

bool RZUF3_Game::copyToClipboard(std::string text)
{
	return SDL_SetClipboardText(text.c_str()) == 0;
}

void RZUF3_Game::setScene(RZUF3_SceneDefinition* sceneDefinition)
{
	if (this->m_scene != nullptr)
	{
		delete this->m_scene;
	}

	if (sceneDefinition == nullptr)
	{
		this->m_scene = nullptr;
		return;
	}

	this->m_scene = new RZUF3_Scene(sceneDefinition, this->m_renderer);
	this->m_scene->init();
}

void RZUF3_Game::getWindowSize(int* width, int* height) const
{
	SDL_GetWindowSize(m_window, width, height);
}

std::string RZUF3_Game::getClipboardText()
{
	char* text = SDL_GetClipboardText();
	if (text == nullptr) return "";
	std::string result(text);
	SDL_free(text);

	return result;
}

RZUF3_ConfigFile* RZUF3_Game::getConfigFile(std::string id)
{
	if (id == "") {
		if (m_defaultConfigFile == nullptr) {
			spdlog::warn("Trying to access default config file, but none is set");
			return nullptr;
		}

		return m_defaultConfigFile;
	}

	auto it = m_configFiles.find(id);
	if (it == m_configFiles.end()) {
		spdlog::warn("Config file {} not found", id);
		return nullptr;
	}

	return it->second;
}

void RZUF3_Game::update(double dt)
{
	if (m_scene != nullptr)
	{
		RZUF3_UpdateEvent rEvent(dt);
		m_scene->getEventsManager()->dispatchEvent(&rEvent);
	}

	m_counter++;
}

void RZUF3_Game::render(double dt)
{
	SDL_SetRenderDrawColor(m_sdlRenderer, m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
	if(m_clearColor.a != 0) SDL_RenderClear(m_sdlRenderer);

	if (m_scene != nullptr)
	{
		RZUF3_DrawEvent rEvent(dt);
		m_scene->getEventsManager()->dispatchEvent(&rEvent);
	}
	SDL_RenderPresent(m_sdlRenderer);
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
		case SDL_MOUSEWHEEL:
			rEvent = new RZUF3_MouseWheelEvent(sdlEvent.wheel.x, sdlEvent.wheel.y);
			break;
		case SDL_TEXTINPUT:
			rEvent = new RZUF3_TextInputEvent(sdlEvent.text.text);
			break;
		case SDL_TEXTEDITING:
			rEvent = new RZUF3_TextEditingEvent(sdlEvent.edit.text, sdlEvent.edit.start, sdlEvent.edit.length);
			break;
		case SDL_WINDOWEVENT:
			if (sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				int width = sdlEvent.window.data1;
				int height = sdlEvent.window.data2;

				spdlog::debug("Window resized to {}x{}", width, height);
				m_renderer->setWindowSize(width, height);
				rEvent = new RZUF3_WindowResizeEvent(width, height);
				break;
			}
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

	if(m_defaultConfigFile != nullptr) m_defaultConfigFile = nullptr;
	for (auto& config : m_configFiles)
	{
		delete config.second;
	}
}
