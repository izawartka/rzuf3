#pragma once
#include "../events/draw.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

struct RZUF3_TextureRendererOptions {
	std::string filepath;
	int x = 0;
	int y = 0;
	SDL_Rect srcRect = { 0, 0, 0, 0 };
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
};

class RZUF3_TextureRenderer : public RZUF3_ObjectScript {
public:
	RZUF3_TextureRenderer(std::string filepath);
	RZUF3_TextureRenderer(RZUF3_TextureRendererOptions options);
	~RZUF3_TextureRenderer();

	void init();
	void deinit();

	void setFilepath(std::string filepath);
	void setDstRect(SDL_Rect dstRect);
	void setDstPos(int x, int y);
	void setSrcRect(SDL_Rect srcRect);
	void setAlign(RZUF3_Align alignment);
	void clearSrcRect();

protected:
	void onDraw(RZUF3_DrawEvent* event);

	void removeTexture();
	void updateTexture();

	RZUF3_TextureRendererOptions mp_options;

	std::string m_textureFilepath;
	SDL_Texture* m_texture = nullptr;
	SDL_Rect m_dstRect = { 0, 0, 0, 0 };
	SDL_Rect m_srcRect = { 0, 0, 0, 0 };
	RZUF3_Align m_alignment = RZUF3_Align_TopLeft;

	_DECLARE_LISTENER(Draw)
};