#pragma once
#include "../events/draw.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

struct RZUF3_TextStyle {
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Color bgColor = { 0, 0, 0, 0 };
	int size = 16;
	int style = TTF_STYLE_NORMAL;
	int wrapLength = 0;
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
	bool useLangFile = true;
};

struct RZUF3_TextRendererOptions {
	std::string fontFilepath;
	std::string text;
	int x = 0;
	int y = 0;
	SDL_Rect cropRect = { 0, 0, 0, 0 };
	RZUF3_TextStyle style;
	bool enabled = true;
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
	void setMaxSize(int maxWidth, int maxHeight);
	void setOffset(int offsetX, int offsetY);
	void setText(std::string text);
	void setStyle(RZUF3_TextStyle style);
	void setEnabled(bool enabled);
	int pointToCharIndex(int x, int y);

	RZUF3_TextStyle getStyle() const;
	int getWidth() const;
	int getHeight() const;
	bool getEnabled() const;

protected:
	void onDraw(RZUF3_DrawEvent* event);

	void removeFont();
	void updateFont();
	void removeTexture();
	void updateTexture();
	void cacheLangFileText();

	RZUF3_TextRendererOptions mp_options;

	std::string m_fontFilepath;
	TTF_Font* m_font = nullptr;
	std::string m_text;
	std::string m_cachedText;
	RZUF3_TextStyle m_style;
	bool m_enabled = true;
	bool m_metricsOnly = false;
	SDL_Rect m_origSize = { 0, 0, 0, 0 };
	SDL_Rect m_dstRect = { 0, 0, 0, 0 };
	SDL_Rect m_cropRect = { 0, 0, 0, 0 };
	RZUF3_Renderer* m_renderer = nullptr;
	SDL_Texture* m_texture = nullptr;
	
	_DECLARE_LISTENER(Draw)
};