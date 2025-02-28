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
	RZUF3_Renderer(SDL_Renderer* renderer);
	~RZUF3_Renderer();

	void setColor(SDL_Color color);
	void setAlign(RZUF3_Align alignment) { m_alignment = alignment; }
	void setWindowSize(int width, int height) { m_winWidth = width; m_winHeight = height; }
	void setUseObjectPos(bool useObjectPos) { m_useObjectPos = useObjectPos; }
	void setOpacity(Uint8 opacity);
	void resetOpacity();

	void drawLine(RZUF3_Object* parentObject, int x1, int y1, int x2, int y2);
	void fillRect(RZUF3_Object* parentObject, SDL_Rect rect);
	void drawRect(RZUF3_Object* parentObject, SDL_Rect rect, unsigned int width);
	void drawTexture(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dstRect);
	void drawTexture(RZUF3_Object* parentObject, SDL_Texture* texture, SDL_Rect* srcRect, SDL_FRect dstRect);
	void fillCircle(RZUF3_Object* parentObject, SDL_Rect rect);
	bool createStaticTexture(SDL_Texture* &texture, int width, int height);
	bool createCacheTexture(SDL_Texture* &texture, int width, int height, std::function<void()> drawFunction);
	bool getPixel(SDL_Texture* texture, int x, int y, uint32_t &pixel);
	bool isRectOnScreen(SDL_Rect& rect, bool fully = false) const;

	SDL_Renderer* getSDLRenderer() { return m_renderer; }
	bool getUseObjectPos() const { return m_useObjectPos; }

	static void screenToObjectXY(RZUF3_Object* parentObject, int& x, int& y);
	static void screenToObjectXY(RZUF3_Object* parentObject, float& x, float& y);
	static void objectToScreenXY(RZUF3_Object* parentObject, int& x, int& y);
	static void objectToScreenXY(RZUF3_Object* parentObject, float& x, float& y);
	static void objectToScreenRect(RZUF3_Object* parentObject, SDL_Rect& rect);
	static void objectToScreenRect(RZUF3_Object* parentObject, SDL_FRect& rect);
	static void screenToRectXY(RZUF3_Object* parentObject, SDL_Rect& rect, int& x, int& y);
	static void screenToRectXY(RZUF3_Object* parentObject, SDL_FRect& rect, float& x, float& y);
	static bool isXYInside(SDL_Rect& rect, int x, int y, bool ignoreRectXY = false);
	static bool isXYInside(SDL_FRect& rect, float x, float y, bool ignoreRectXY = false);
	static void alignRect(SDL_Rect& rect, RZUF3_Align alignment, SDL_Rect* refRect = nullptr);
	static void alignRect(SDL_FRect& rect, RZUF3_Align alignment, SDL_FRect* refRect = nullptr);

private:

	SDL_Renderer* m_renderer;
	RZUF3_Align m_alignment = RZUF3_Align_Left;
	bool m_useObjectPos = true;
	int m_winWidth = 0;
	int m_winHeight = 0;
};

extern RZUF3_Renderer* g_renderer;