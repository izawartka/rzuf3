#include "renderer.h"
#include "renderer.h"
#include "renderer.h"
#include "renderer.h"

void RZUF3_Renderer::setColor(SDL_Color color)
{
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}

void RZUF3_Renderer::fillRect(RZUF3_Object* parentObject, SDL_Rect rect)
{
	objectToScreenRect(parentObject, rect);
	alignRect(rect, m_alignment);
	SDL_RenderFillRect(m_renderer, &rect);
}

void RZUF3_Renderer::drawRect(RZUF3_Object* parentObject, SDL_Rect rect, unsigned int borderWidth)
{
	objectToScreenRect(parentObject, rect);
	alignRect(rect, m_alignment);
	RZUF3_Pos pos = parentObject->getAbsolutePos();

	int borderWidthX = borderWidth * pos.scaleX;
	int borderWidthY = borderWidth * pos.scaleY;

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
	alignRect(dstRect, m_alignment);
	SDL_RenderCopy(m_renderer, texture, srcRect, &dstRect);
}

void RZUF3_Renderer::drawTextureOpaque(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect, Uint8 opacity)
{
	objectToScreenRect(parentObject, dstRect);
	alignRect(dstRect, m_alignment);
	SDL_SetTextureAlphaMod(texture, opacity);
	SDL_RenderCopy(m_renderer, texture, srcRect, &dstRect);
	SDL_SetTextureAlphaMod(texture, 255);
}

void RZUF3_Renderer::fillCircle(RZUF3_Object* parentObject, SDL_Rect rect)
{
	objectToScreenRect(parentObject, rect);
	alignRect(rect, m_alignment);

	int rx = rect.w / 2;
	int ry = rect.h / 2;
	int cx = rect.x + rx;
	int cy = rect.y + ry;

	for (int x = -rx; x <= rx; x++)
	{
		double scX = (double)x / rx;
		double scY = sqrt(1 - scX * scX);
		int y = scY * ry;

		SDL_RenderDrawLine(m_renderer, 
			cx + x, cy - y, 
			cx + x, cy + y
		);
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

void RZUF3_Renderer::alignRect(SDL_Rect& rect, RZUF3_Align alignment)
{
	switch (alignment)
	{
	case RZUF3_Align_TopLeft:
		break;
	case RZUF3_Align_Top:
		rect.x += rect.w / 2;
		break;
	case RZUF3_Align_TopRight:
		rect.x += rect.w;
		break;
	case RZUF3_Align_Left:
		rect.y += rect.h / 2;
		break;
	case RZUF3_Align_Center:
		rect.x += rect.w / 2;
		rect.y += rect.h / 2;
		break;
	case RZUF3_Align_Right:
		rect.x += rect.w;
		rect.y += rect.h / 2;
		break;
	case RZUF3_Align_BottomLeft:
		rect.y += rect.h;
		break;
	case RZUF3_Align_Bottom:
		rect.x += rect.w / 2;
		rect.y += rect.h;
		break;
	case RZUF3_Align_BottomRight:
		rect.x += rect.w;
		rect.y += rect.h;
		break;
	}
}