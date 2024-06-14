#include "texture.h"
#include "../renderer.h"

RZUF3_Texture* RZUF3_Texture::getInstance(std::string filepath)
{
	RZUF3_Texture* instance = new RZUF3_Texture();
	instance->m_filepath = filepath;
	return instance;
}

bool RZUF3_Texture::load()
{
	SDL_Surface* surface = IMG_Load(m_filepath.c_str());
	if (surface == nullptr)
	{
		spdlog::error("Texture {}: failed to load image: {} ", m_filepath, IMG_GetError());
		return false;
	}

	SDL_Renderer* renderer = g_renderer->getSDLRenderer();
	m_texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (m_texture == nullptr)
	{
		spdlog::error("Texture {}: failed to create texture: {}", m_filepath, SDL_GetError());
		return false;
	}

	SDL_FreeSurface(surface);

	spdlog::info("Loaded texture: {}", m_filepath);

	return true;
}

void RZUF3_Texture::unload()
{
	SDL_DestroyTexture(m_texture);

	spdlog::info("Unloaded texture: {}", m_filepath);
}

void* RZUF3_Texture::getContent()
{
	return m_texture;
}