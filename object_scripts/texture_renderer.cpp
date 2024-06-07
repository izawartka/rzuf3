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
	this->m_textureFilepath = mp_options.filepath;
	this->m_dstRect = {mp_options.x, mp_options.y, 0, 0};
	this->m_srcRect = this->mp_options.srcRect;
	this->m_renderer = this->getObject()->getScene()->getRenderer();

	this->updateTexture();

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
}

void RZUF3_TextureRenderer::deinit()
{
	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);

	removeTexture();

	m_renderer = nullptr;
}

void RZUF3_TextureRenderer::setFilepath(std::string filepath)
{
	removeTexture();
	this->m_textureFilepath = filepath;
	updateTexture();
}

void RZUF3_TextureRenderer::setDstRect(SDL_Rect dstRect)
{
	this->m_dstRect = dstRect;
}

void RZUF3_TextureRenderer::setDstPos(int x, int y)
{
	this->m_dstRect.x = x;
	this->m_dstRect.y = y;
}

void RZUF3_TextureRenderer::setSrcRect(SDL_Rect srcRect)
{
	this->m_srcRect = srcRect;
}

void RZUF3_TextureRenderer::clearSrcRect()
{
	this->m_srcRect = { 0, 0, 0, 0 };
}

void RZUF3_TextureRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_object == nullptr) return;
	if (m_texture == nullptr) return;

	SDL_Rect* srcRectPtr = nullptr;
	if (m_srcRect.w > 0 && m_srcRect.h > 0) {
		srcRectPtr = &m_srcRect;
	}

	m_renderer->drawTexture(m_object, m_texture, srcRectPtr, m_dstRect);
}

void RZUF3_TextureRenderer::removeTexture()
{
	if (m_texture == nullptr) return;

	RZUF3_AssetsManager* assetsManager = this->m_object->getScene()->getAssetsManager();

	assetsManager->removeAsset(m_textureFilepath);
	m_texture = nullptr;
}

void RZUF3_TextureRenderer::updateTexture()
{
	if (this->m_textureFilepath == "") return;

	RZUF3_AssetsManager* assetsManager = this->m_object->getScene()->getAssetsManager();

	RZUF3_AssetDefinition assetDefinition;
	assetDefinition.filepath = m_textureFilepath;
	assetDefinition.factory = RZUF3_Texture::getInstance;

	if (!assetsManager->addAsset(assetDefinition)) return;

	this->m_texture = (SDL_Texture*)assetsManager->getAssetContent(m_textureFilepath);

	if(this->m_texture == nullptr)
	{
		spdlog::error("Texture {} not found", this->m_textureFilepath);
		return;
	}

	SDL_QueryTexture(this->m_texture, nullptr, nullptr, &this->m_dstRect.w, &this->m_dstRect.h);
}