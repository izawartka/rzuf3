#include "renderer.h"
#include "renderer.h"
#include "renderer.h"
#include "renderer.h"

RZUF3_Renderer* g_renderer = nullptr;

RZUF3_Renderer::RZUF3_Renderer(SDL_Renderer* renderer)
{
	if(g_renderer != nullptr) throw std::logic_error("Only one instance of RZUF3_Renderer can be present at a time");
	g_renderer = this;

	m_renderer = renderer;
}

RZUF3_Renderer::~RZUF3_Renderer()
{
	g_renderer = nullptr;
}

void RZUF3_Renderer::setColor(SDL_Color color)
{
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

	SDL_BlendMode blendMode = color.a != 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE;
	SDL_SetRenderDrawBlendMode(m_renderer, blendMode);
}

void RZUF3_Renderer::drawLine(RZUF3_Object* parentObject, int x1, int y1, int x2, int y2)
{
	int x1c = x1;
	int y1c = y1;
	int x2c = x2;
	int y2c = y2;

	if (m_useObjectPos && parentObject != nullptr)
	{
		screenToObjectXY(parentObject, x1c, y1c);
		screenToObjectXY(parentObject, x2c, y2c);
	}
	SDL_RenderDrawLine(m_renderer, x1c, y1c, x2c, y2c);
}

void RZUF3_Renderer::fillRect(RZUF3_Object* parentObject, SDL_Rect rect)
{
	if (m_useObjectPos && parentObject != nullptr)
	{
		objectToScreenRect(parentObject, rect);
	}
	alignRect(rect, m_alignment);
	SDL_RenderFillRect(m_renderer, &rect);
}

void RZUF3_Renderer::drawRect(RZUF3_Object* parentObject, SDL_Rect rect, unsigned int borderWidth)
{
	double scaleX = 1.0;
	double scaleY = 1.0;

	if (m_useObjectPos && parentObject != nullptr)
	{
		objectToScreenRect(parentObject, rect);
		RZUF3_Pos pos = parentObject->getAbsolutePos();
		scaleX = pos.scaleX;
		scaleY = pos.scaleY;
	}
	alignRect(rect, m_alignment);

	int borderWidthX = borderWidth * scaleX;
	int borderWidthY = borderWidth * scaleY;

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
	if (m_useObjectPos && parentObject != nullptr)
	{
		objectToScreenRect(parentObject, dstRect);
	}
	alignRect(dstRect, m_alignment);
	SDL_RenderCopy(m_renderer, texture, srcRect, &dstRect);
}

void RZUF3_Renderer::drawTextureOpaque(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect, Uint8 opacity)
{
	if (m_useObjectPos && parentObject != nullptr)
	{
		objectToScreenRect(parentObject, dstRect);
	}
	alignRect(dstRect, m_alignment);
	SDL_SetTextureAlphaMod(texture, opacity);
	SDL_RenderCopy(m_renderer, texture, srcRect, &dstRect);
	SDL_SetTextureAlphaMod(texture, 255);
}

void RZUF3_Renderer::fillCircle(RZUF3_Object* parentObject, SDL_Rect rect)
{
	if (m_useObjectPos && parentObject != nullptr)
	{
		objectToScreenRect(parentObject, rect);
	}
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

bool RZUF3_Renderer::createStaticTexture(SDL_Texture* &texture, int width, int height)
{
	if (texture != nullptr) {
		spdlog::error("createStaticTexture: Target texture should be nullptr");
		return false;
	}

	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
	if (surface == nullptr) {
		spdlog::error("createStaticTexture: Failed to create surface: {}", SDL_GetError());
		return false;
	}

	SDL_RenderReadPixels(
		g_renderer->getSDLRenderer(),
		nullptr,
		SDL_PIXELFORMAT_RGBA8888,
		surface->pixels,
		surface->pitch
	);

	texture = SDL_CreateTextureFromSurface(g_renderer->getSDLRenderer(), surface);

	if (texture == nullptr) {
		spdlog::error("createStaticTexture: Failed to create texture: {}", SDL_GetError());
		SDL_FreeSurface(surface);
		return false;
	}

	SDL_FreeSurface(surface);

	return true;
}

bool RZUF3_Renderer::createCacheTexture(SDL_Texture*& texture, int width, int height, std::function<void()> drawFunction)
{
	if (texture != nullptr)
	{
		spdlog::error("RZUF3_Renderer: Target cached texture should be nullptr");
		return false;
	}

	SDL_Texture* tempTexture = SDL_CreateTexture(
		m_renderer,
		SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET,
		width,
		height
	);

	if (tempTexture == nullptr)
	{
		spdlog::error("RZUF3_Renderer: Failed to create temporary texture for caching: {}", SDL_GetError());
		return false;
	}

	SDL_Texture* prevTarget = SDL_GetRenderTarget(m_renderer);
	SDL_SetRenderTarget(m_renderer, tempTexture);
	bool prevUseObjectPos = getUseObjectPos();
	setUseObjectPos(false);
	setAlign(RZUF3_Align_TopLeft);

	drawFunction();

	bool staticTextureSuccess = createStaticTexture(texture, width, height);
	if (!staticTextureSuccess) {
		spdlog::error("RZUF3_Renderer: Failed to create static texture for caching");
	}

	setUseObjectPos(prevUseObjectPos);
	SDL_SetRenderTarget(m_renderer, prevTarget);
	SDL_DestroyTexture(tempTexture);

	return texture != nullptr && staticTextureSuccess;
}

bool RZUF3_Renderer::getPixel(SDL_Texture* texture, int x, int y, uint32_t& pixel)
{
	SDL_Texture* tempTexture = SDL_CreateTexture(
		m_renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		1,
		1
	);

	if (tempTexture == nullptr)
	{
		spdlog::error("RZUF3_Renderer: Failed to create temporary texture for getPixel: {}", SDL_GetError());
		return false;
	}

	SDL_Texture* prevTarget = SDL_GetRenderTarget(m_renderer);
	SDL_SetRenderTarget(m_renderer, tempTexture);
	bool prevUseObjectPos = getUseObjectPos();
	setUseObjectPos(false);
	setAlign(RZUF3_Align_TopLeft);

	SDL_Rect dstRect = { 0, 0, 1, 1 };
	SDL_Rect srcRect = { x, y, 1, 1 };
	SDL_RenderCopy(m_renderer, texture, &srcRect, &dstRect);

	SDL_RenderReadPixels(
		m_renderer,
		&dstRect,
		SDL_PIXELFORMAT_RGBA8888,
		&pixel,
		sizeof(pixel)
	);

	setUseObjectPos(prevUseObjectPos);
	SDL_SetRenderTarget(m_renderer, prevTarget);
	SDL_DestroyTexture(tempTexture);

	return true;
}

bool RZUF3_Renderer::isRectOnScreen(SDL_Rect& rect, bool fully) const
{
	int nodesOnScreen = 0;
	int x2 = rect.x + rect.w;
	int y2 = rect.y + rect.h;

	if (rect.x >= 0 && rect.x < m_winWidth) nodesOnScreen++;
	if (rect.y >= 0 && rect.y < m_winHeight) nodesOnScreen++;
	if (x2 >= 0 && x2 < m_winWidth) nodesOnScreen++;
	if (y2 >= 0 && y2 < m_winHeight) nodesOnScreen++;

	if (fully) return nodesOnScreen == 4;
	return nodesOnScreen > 0;
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

void RZUF3_Renderer::screenToRectXY(RZUF3_Object* parentObject, SDL_Rect& rect, int& x, int& y)
{
	screenToObjectXY(parentObject, x, y);

	x -= rect.x;
	y -= rect.y;
}

bool RZUF3_Renderer::isXYInside(SDL_Rect& rect, int x, int y, bool ignoreRectXY)
{
	if(!ignoreRectXY)
	{
		x -= rect.x;
		y -= rect.y;
	}

	if (x < 0 || x >= rect.w) return false;
	if (y < 0 || y >= rect.h) return false;

	return true;
}

void RZUF3_Renderer::alignRect(SDL_Rect& rect, RZUF3_Align alignment, SDL_Rect* refRect)
{
	if(refRect == nullptr) refRect = &rect;

	switch (alignment)
	{
	case RZUF3_Align_TopLeft:
		break;
	case RZUF3_Align_Top:
		rect.x -= refRect->w / 2;
		break;
	case RZUF3_Align_TopRight:
		rect.x -= refRect->w;
		break;
	case RZUF3_Align_Left:
		rect.y -= refRect->h / 2;
		break;
	case RZUF3_Align_Center:
		rect.x -= refRect->w / 2;
		rect.y -= refRect->h / 2;
		break;
	case RZUF3_Align_Right:
		rect.x -= refRect->w;
		rect.y -= refRect->h / 2;
		break;
	case RZUF3_Align_BottomLeft:
		rect.y -= refRect->h;
		break;
	case RZUF3_Align_Bottom:
		rect.x -= refRect->w / 2;
		rect.y -= refRect->h;
		break;
	case RZUF3_Align_BottomRight:
		rect.x -= refRect->w;
		rect.y -= refRect->h;
		break;
	}
}