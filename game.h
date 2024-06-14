#pragma once
#include "common.h"
#include "scene_definition.h"
#include "scene.h"
#include "renderer.h"
#include "events/events.h"
#include "lang.h"

#define RZUF3_GAME_FPS 60

class RZUF3_Game {
public:
	RZUF3_Game();
	~RZUF3_Game();

	void loadLanguage(std::string filepath);
	void initWindow(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void startGameLoop();
	void handleSDLEvents();
	void clean();
	void setScene(RZUF3_SceneDefinition* sceneDefinition);
	RZUF3_Scene* getScene();
	bool setWindowIcon(std::string filepath);
	void addGlobalEvents();

	void update(double dt);
	void render(double dt);

	bool isRunning() const { return m_isRunning; }
	RZUF3_Renderer* getRenderer() { return m_renderer; }
private:
	void onQuit(RZUF3_QuitEvent* event);
	void onSetWindowSize(RZUF3_SetWindowSizeEvent* event);
	void onSetWindowIcon(RZUF3_SetWindowIconEvent* event);
	void onSetWindowTitle(RZUF3_SetWindowTitleEvent* event);
	void onSetWindowFullscreen(RZUF3_SetWindowFullscreenEvent* event);
	void onSetScene(RZUF3_SetSceneEvent* event);

	bool m_isRunning = false;
	SDL_Window* m_window = nullptr;
	RZUF3_Renderer* m_renderer = nullptr;
	SDL_Renderer* m_sdlRenderer = nullptr;
	unsigned int m_counter = 0;
	RZUF3_Scene* m_scene = nullptr;
	RZUF3_Lang* m_lang = nullptr;
};

extern RZUF3_Game* g_game;