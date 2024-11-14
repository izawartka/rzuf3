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
	void initWindow(int width, int height, bool fullscreen);
	void startGameLoop();

	void quit();
	void setWindowTitle(std::string title, bool useLangFile = true);
	void setWindowSize(int width, int height);
	void setWindowFullscreen(bool fullscreen);
	void setWindowResizable(bool resizable);
	void setWindowMinimumSize(int width, int height);
	void setWindowMaximumSize(int width, int height);
	bool setWindowIcon(std::string filepath);
	void setRelativeMouseMode(bool enabled);
	void setCursorPos(int x, int y);
	void setCursorVisible(bool visible);
	void setClearColor(SDL_Color color) { m_clearColor = color; }
	void requestTextInput(std::string id, SDL_Rect rect);
	void stopTextInput(std::string id);

	void setScene(RZUF3_SceneDefinition* sceneDefinition);

	bool isRunning() const { return m_isRunning; }
	void getWindowSize(int* width, int* height) const;

private:
	void update(double dt);
	void render(double dt);
	void handleSDLEvents();
	void clean();

	bool m_isRunning = false;
	SDL_Window* m_window = nullptr;
	RZUF3_Renderer* m_renderer = nullptr;
	SDL_Renderer* m_sdlRenderer = nullptr;
	unsigned int m_counter = 0;
	RZUF3_Scene* m_scene = nullptr;
	RZUF3_Lang* m_lang = nullptr;
	std::string m_currentTextInputId = "";
	SDL_Color m_clearColor = { 0, 0, 0, 255 };
};

extern RZUF3_Game* g_game;