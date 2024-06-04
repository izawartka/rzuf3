#pragma once
#include "common.h"
#include "object.h"
#include "pos.h"

class RZUF3_Object;

class RZUF3_Renderer {
public:
	RZUF3_Renderer(SDL_Renderer* renderer) : m_renderer(renderer) {}
	~RZUF3_Renderer() {}

	void fillRect(RZUF3_Object* parentObject, SDL_Rect rect, SDL_Color color);
	void drawRect(RZUF3_Object* parentObject, SDL_Rect rect, SDL_Color color, unsigned int width);
	void drawTexture(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect);
	void drawTextureOpaque(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect, Uint8 opacity);
	void fillCircle(RZUF3_Object* parentObject, RZUF3_Pos pos, int radius, SDL_Color color);

	SDL_Renderer* getSDLRenderer() { return m_renderer; }

private:
	void translatePos(RZUF3_Object* parentObject, RZUF3_Pos& pos);
	void translateRect(RZUF3_Object* parentObject, SDL_Rect& rect);
	SDL_Renderer* m_renderer;
};