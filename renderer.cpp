#include "renderer.h"
#include "renderer.h"
#include "renderer.h"
#include "renderer.h"

void RZUF3_Renderer::fillRect(RZUF3_Object* parentObject, SDL_Rect rect, SDL_Color color)
{
	translateRect(parentObject, rect);
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(m_renderer, &rect);
}

void RZUF3_Renderer::drawRect(RZUF3_Object* parentObject, SDL_Rect rect, SDL_Color color, unsigned int width)
{
	translateRect(parentObject, rect);

	for(unsigned int i = 0; i < width; i++)
	{
		SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawRect(m_renderer, &rect);
		rect.x++;
		rect.y++;
		rect.w -= 2;
		rect.h -= 2;
	}
}

void RZUF3_Renderer::drawTexture(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect)
{
	translateRect(parentObject, dstRect);
	SDL_RenderCopy(m_renderer, texture, srcRect, &dstRect);
}

void RZUF3_Renderer::fillCircle(RZUF3_Object* parentObject, RZUF3_Pos pos, int radius, SDL_Color color)
{
	translatePos(parentObject, pos);
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

	for(int w = 0; w < radius * 2; w++)
	{
		for(int h = 0; h < radius * 2; h++)
		{
			int dx = radius - w;
			int dy = radius - h;

			if((dx * dx + dy * dy) <= (radius * radius))
			{
				SDL_RenderDrawPoint(m_renderer, pos.x + dx, pos.y + dy);
			}
		}
	}
}

void RZUF3_Renderer::translatePos(RZUF3_Object* parentObject, RZUF3_Pos& pos)
{
	RZUF3_Pos parentPos = parentObject->getPos();

	pos.x += parentPos.x;
	pos.y += parentPos.y;
}

void RZUF3_Renderer::translateRect(RZUF3_Object* parentObject, SDL_Rect& rect)
{
	RZUF3_Pos pos = parentObject->getPos();

	rect.x += pos.x;
	rect.y += pos.y;
}
