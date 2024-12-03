#include "font.h"
#include "../renderer.h"

RZUF3_Font* RZUF3_Font::getInstance(std::string filepath)
{
	RZUF3_Font* instance = new RZUF3_Font();
	instance->m_filepath = filepath;
	return instance;
}

bool RZUF3_Font::load()
{
	m_font = TTF_OpenFont(m_filepath.c_str(), 24);
	if (m_font == nullptr)
	{
		spdlog::error("Font {}: {}", m_filepath, TTF_GetError());
		return false;
	}

	spdlog::info("Loaded font: {}", m_filepath);
	return true;
}

void RZUF3_Font::unload()
{
	TTF_CloseFont(m_font);

	spdlog::info("Unloaded font: {}", m_filepath);
}

void* RZUF3_Font::getContent()
{
	return m_font;
}
