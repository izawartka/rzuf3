#include "texture_renderer.h"
#include "../object.h"
#include "../scene.h"
#include "../asset.h"
#include "../assets/texture.h"
#include "../events_manager.h"

RZUF3_TextureRenderer::RZUF3_TextureRenderer(std::string filepath)
{
	mp_options.filepath = filepath;
}

RZUF3_TextureRenderer::RZUF3_TextureRenderer(RZUF3_TextureRendererOptions options)
{
	mp_options = options;
}

RZUF3_TextureRenderer::~RZUF3_TextureRenderer()
{

}

void RZUF3_TextureRenderer::init()
{
	m_textureFilepath = mp_options.filepath;
	m_dstRect = {mp_options.x, mp_options.y, 0, 0};
	m_srcRect = mp_options.srcRect;
	m_alignment = mp_options.alignment;

	updateTexture();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
}

void RZUF3_TextureRenderer::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);

	removeTexture();
}

void RZUF3_TextureRenderer::setFilepath(std::string filepath)
{
	removeTexture();
	m_textureFilepath = filepath;
	updateTexture();
}

void RZUF3_TextureRenderer::setDstRect(SDL_Rect dstRect)
{
	m_dstRect = dstRect;
}

void RZUF3_TextureRenderer::setDstPos(int x, int y)
{
	m_dstRect.x = x;
	m_dstRect.y = y;
}

void RZUF3_TextureRenderer::setSrcRect(SDL_Rect srcRect)
{
	m_srcRect = srcRect;
}

void RZUF3_TextureRenderer::setAlign(RZUF3_Align alignment)
{
	m_alignment = alignment;
}

void RZUF3_TextureRenderer::clearSrcRect()
{
	m_srcRect = { 0, 0, 0, 0 };
}

void RZUF3_TextureRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_object == nullptr) return;
	if (m_texture == nullptr) return;

	SDL_Rect* srcRectPtr = nullptr;
	if (m_srcRect.w > 0 && m_srcRect.h > 0) {
		srcRectPtr = &m_srcRect;
	}

	g_renderer->setAlign(m_alignment);
	g_renderer->drawTexture(m_object, m_texture, srcRectPtr, m_dstRect);
}

void RZUF3_TextureRenderer::removeTexture()
{
	if (m_texture == nullptr) return;

	RZUF3_AssetsManager* assetsManager = g_scene->getAssetsManager();

	assetsManager->removeAsset(m_textureFilepath);
	m_texture = nullptr;
}

void RZUF3_TextureRenderer::updateTexture()
{
	if (m_textureFilepath == "") return;

	RZUF3_AssetsManager* assetsManager = g_scene->getAssetsManager();

	RZUF3_AssetDefinition assetDefinition;
	assetDefinition.filepath = m_textureFilepath;
	assetDefinition.factory = RZUF3_Texture::getInstance;

	RZUF3_Texture* textureAsset = (RZUF3_Texture*)assetsManager->addAsset(assetDefinition);

	if (textureAsset == nullptr)
	{
		spdlog::error("Texture {} could not be loaded", m_textureFilepath);
		return;
	}

	m_texture = (SDL_Texture*)textureAsset->getContent();

	SDL_QueryTexture(m_texture, nullptr, nullptr, &m_dstRect.w, &m_dstRect.h);
}