#include "renderer.h"
#include "renderer.h"
#include "renderer.h"
#include "renderer.h"

void RZUF3_Renderer::fillRect(RZUF3_Object* parentObject, SDL_Rect rect, SDL_Color color)
{
	objectToScreenRect(parentObject, rect);
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(m_renderer, &rect);
}

void RZUF3_Renderer::drawRect(RZUF3_Object* parentObject, SDL_Rect rect, SDL_Color color, unsigned int borderWidth)
{
	objectToScreenRect(parentObject, rect);
	RZUF3_Pos pos = parentObject->getAbsolutePos();

	int borderWidthX = borderWidth * pos.scaleX;
	int borderWidthY = borderWidth * pos.scaleY;

	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

	for (int i = 0; i < borderWidthX; i++)
	{
		SDL_RenderDrawLine(m_renderer, rect.x + i, rect.y, rect.x + i, rect.y + rect.h - 1);
		SDL_RenderDrawLine(m_renderer, rect.x + rect.w - 1 - i, rect.y, rect.x + rect.w - 1 - i, rect.y + rect.h - 1);
	}

	for (int i = 0; i < borderWidthY; i++)
	{
		SDL_RenderDrawLine(m_renderer, rect.x, rect.y + i, rect.x + rect.w - 1, rect.y + i);
		SDL_RenderDrawLine(m_renderer, rect.x, rect.y + rect.h - 1 - i, rect.x + rect.w - 1, rect.y + rect.h - 1 - i);
	}
}

void RZUF3_Renderer::drawTexture(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect)
{
	objectToScreenRect(parentObject, dstRect);
	SDL_RenderCopy(m_renderer, texture, srcRect, &dstRect);
}

void RZUF3_Renderer::drawTextureOpaque(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect, Uint8 opacity)
{
	objectToScreenRect(parentObject, dstRect);
	SDL_SetTextureAlphaMod(texture, opacity);
	SDL_RenderCopy(m_renderer, texture, srcRect, &dstRect);
	SDL_SetTextureAlphaMod(texture, 255);
}

void RZUF3_Renderer::fillCircle(RZUF3_Object* parentObject, int cx, int cy, int radius, SDL_Color color)
{
	objectToScreenXY(parentObject, cx, cy);
	RZUF3_Pos pos = parentObject->getAbsolutePos();

	int radiusX = radius * pos.scaleX;
	int radiusY = radius * pos.scaleY;

	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

	for (int x = -radiusX; x <= radiusX; x++)
	{
		double scX = x / pos.scaleX;
		double scY = sqrt(radiusY * radiusY - scX * scX);
		int y = scY * pos.scaleY;

		SDL_RenderDrawLine(m_renderer, cx + x, cy + y, cx + x, cy - y);
	}
}

void RZUF3_Renderer::screenToObjectXY(RZUF3_Object* parentObject, int& x, int& y)
{
	RZUF3_Pos objectPos = parentObject->getAbsolutePos();

	x = (x - objectPos.x) / objectPos.scaleX;
	y = (y - objectPos.y) / objectPos.scaleY;
}

void RZUF3_Renderer::objectToScreenXY(RZUF3_Object* parentObject, int &x, int &y)
{
	RZUF3_Pos parentPos = parentObject->getAbsolutePos();

	x = parentPos.x + x * parentPos.scaleX;
	y = parentPos.y + y * parentPos.scaleY;
}

void RZUF3_Renderer::objectToScreenRect(RZUF3_Object* parentObject, SDL_Rect& rect)
{
	RZUF3_Pos pos = parentObject->getAbsolutePos();

	rect.x = pos.x + rect.x * pos.scaleX;
	rect.y = pos.y + rect.y * pos.scaleY;
	rect.w *= pos.scaleX;
	rect.h *= pos.scaleY;
}
