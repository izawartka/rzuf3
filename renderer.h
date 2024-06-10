#pragma once
#include "common.h"
#include "object.h"
#include "pos.h"

class RZUF3_Object;

enum RZUF3_Align {
	RZUF3_Align_TopLeft,
	RZUF3_Align_Top,
	RZUF3_Align_TopRight,
	RZUF3_Align_Left,
	RZUF3_Align_Center,
	RZUF3_Align_Right,
	RZUF3_Align_BottomLeft,
	RZUF3_Align_Bottom,
	RZUF3_Align_BottomRight
};

class RZUF3_Renderer {
public:
	RZUF3_Renderer(SDL_Renderer* renderer) : m_renderer(renderer) {}
	~RZUF3_Renderer() {}

	void setColor(SDL_Color color);
	void setAlign(RZUF3_Align alignment) { m_alignment = alignment; }

	void fillRect(RZUF3_Object* parentObject, SDL_Rect rect);
	void drawRect(RZUF3_Object* parentObject, SDL_Rect rect, unsigned int width);
	void drawTexture(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect);
	void drawTextureOpaque(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect, Uint8 opacity);
	void fillCircle(RZUF3_Object* parentObject, SDL_Rect rect);

	SDL_Renderer* getSDLRenderer() { return m_renderer; }

	static void screenToObjectXY(RZUF3_Object* parentObject, int& x, int& y);
	static void objectToScreenXY(RZUF3_Object* parentObject, int& x, int& y);
	static void objectToScreenRect(RZUF3_Object* parentObject, SDL_Rect& rect);

private:
	static void alignRect(SDL_Rect& rect, RZUF3_Align alignment);

	SDL_Renderer* m_renderer;
	RZUF3_Align m_alignment = RZUF3_Align_Left;
};