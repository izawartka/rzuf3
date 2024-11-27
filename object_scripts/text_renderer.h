#pragma once
#include "../events/draw.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

struct RZUF3_TextStyle {
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Color bgColor = { 0, 0, 0, 0 };
	SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
	int size = 16;
	int style = TTF_STYLE_NORMAL;
	int wrapLength = 0;
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
	bool useLangFile = true;
};

struct RZUF3_TextRendererOptions {
	std::string fontFilepath;
	std::string text;
	SDL_Rect dstRect = { 0, 0, 0, 0 };
	SDL_Rect cropRect = { 0, 0, 0, 0 };
	RZUF3_TextStyle style;
	bool useOnDraw = true;
	bool metricsOnly = false;
};

class RZUF3_TextRenderer : public RZUF3_ObjectScript {
public:
	RZUF3_TextRenderer(std::string fontFilepath, std::string text);
	RZUF3_TextRenderer(RZUF3_TextRendererOptions options);
	~RZUF3_TextRenderer();

	void init();
	void deinit();

	void setFontFilepath(std::string fontFilepath);
	void setDstPos(int x, int y);
	void setDstSize(int width, int height);
	void setOffset(int offsetX, int offsetY);
	void setMaxSize(int maxWidth, int maxHeight);
	void setText(std::string text);
	void setStyle(RZUF3_TextStyle style);
	void setUseOnDraw(bool useOnDraw);
	int pointToCharIndex(int x, int y);
	void draw();

	RZUF3_TextStyle getStyle() const;
	int getWidth() const;
	int getHeight() const;
	bool getUseOnDraw() const;

protected:
	void onDraw(RZUF3_DrawEvent* event);

	void removeFont();
	void updateFont();
	void removeTexture();
	void updateTexture();
	void cacheLangFileText();

	RZUF3_TextRendererOptions mp_options;

	RZUF3_TextRendererOptions m_options;

	TTF_Font* m_font = nullptr;
	std::string m_cachedText;
	SDL_Rect m_origSize = { 0, 0, 0, 0 };
	RZUF3_Renderer* m_renderer = nullptr;
	SDL_Texture* m_texture = nullptr;
	bool hasOnDrawListener = false;
	
	_DECLARE_LISTENER(Draw)
};