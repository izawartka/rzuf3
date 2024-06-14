#pragma once
#include "../common.h"
#include "../object_script.h"
#include "../events/draw.h"
#include "../event_macros.h"

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
	RZUF3_TextStyle style;
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
	void setText(std::string text);
	void setStyle(RZUF3_TextStyle style);

	RZUF3_TextStyle getStyle();
	int getWidth();
	int getHeight();

protected:
	void onDraw(RZUF3_DrawEvent* event);

	void removeFont();
	void updateFont();
	void removeTexture();
	void updateTexture();

	RZUF3_TextRendererOptions mp_options;

	std::string m_fontFilepath;
	TTF_Font* m_font = nullptr;
	std::string m_text;
	RZUF3_TextStyle m_style;
	SDL_Rect m_dstRect = { 0, 0, 0, 0 };
	RZUF3_Renderer* m_renderer = nullptr;
	SDL_Texture* m_texture = nullptr;
	
	_DECLARE_LISTENER(Draw)
};