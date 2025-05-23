#pragma once
#include "../events/draw.h"
#include "../event_macros.h"
#include "text_renderer_style.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_Font;

struct RZUF3_TextRendererOptions {
	std::string text;
	SDL_Rect dstRect = { 0, 0, 0, 0 };
	SDL_Rect cropRect = { 0, 0, 0, 0 };
	RZUF3_TextRendererStyle style;
	bool useOnDraw = true;
	bool metricsOnly = false;
	bool useLangFile = true;
	int wrapLength = 0;
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
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
	void setStyle(RZUF3_TextRendererStyle style);
	void setUseLangFile(bool useLangFile);
	void setUseOnDraw(bool useOnDraw);
	void setWrapLength(int wrapLength);
	void setAlignment(RZUF3_Align alignment);
	void setMetricsOnly(bool metricsOnly);
	int pointToCharIndex(int x, int y);
	bool charIndexToPoint(int index, int& x, int& y);
	void draw();

	SDL_Texture* getTexture() const; // uncropped texture
	RZUF3_TextRendererStyle getStyle() const;
	int getWidth() const;
	int getHeight() const;
	bool getUseOnDraw() const;
	int getFontAscent() const;
	int getFontDescent() const;
	int getFontHeight() const;

protected:
	void onDraw(RZUF3_DrawEvent* event);
	void onLangChange(RZUF3_LangChangeEvent* event);

	void removeFont();
	void createFont();
	void removeTexture();
	void updateTexture();
	void cacheLangFileText();

	RZUF3_TextRendererOptions mp_options;

	RZUF3_TextRendererOptions m_options;

	RZUF3_Font* m_fontAsset = nullptr;
	TTF_Font* m_font = nullptr;
	std::string m_cachedText = "";
	SDL_Rect m_origSize = { 0, 0, 0, 0 };
	SDL_Texture* m_texture = nullptr;
	bool m_hasOnDrawListener = false;
	bool m_hasOnLangChangeListener = false;
	
	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(LangChange)
};