#pragma once
#include "common.h"
#include "scene_definition.h"
#include "scene.h"
#include "renderer.h"

#define RZUF3_GAME_FPS 60

class RZUF3_Game {
public:
	RZUF3_Game();
	~RZUF3_Game();

	void initWindow(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void startGameLoop();
	void handleSDLEvents();
	void clean();
	void setScene(RZUF3_SceneDefinition* sceneDefinition);
	RZUF3_Scene* getScene();

	void update(double dt);
	void render(double dt);

	bool isRunning() const { return m_isRunning; }
	RZUF3_Renderer* getRenderer() { return m_renderer; }

private:
	bool m_isRunning = false;
	SDL_Window* m_window = nullptr;
	RZUF3_Renderer* m_renderer = nullptr;
	SDL_Renderer* m_sdlRenderer = nullptr;
	unsigned int m_counter = 0;
	RZUF3_Scene* m_scene = nullptr;
};