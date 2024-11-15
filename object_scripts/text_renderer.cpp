#include "text_renderer.h"
#include "../lang.h"

RZUF3_TextRenderer::RZUF3_TextRenderer(std::string fontFilepath, std::string text)
{
	mp_options.fontFilepath = fontFilepath;
	mp_options.text = text;
}

RZUF3_TextRenderer::RZUF3_TextRenderer(RZUF3_TextRendererOptions options)
{
	mp_options = options;
}

RZUF3_TextRenderer::~RZUF3_TextRenderer()
{

}

void RZUF3_TextRenderer::init()
{
	m_fontFilepath = mp_options.fontFilepath;
	m_text = mp_options.text;
	m_style = mp_options.style;
	m_dstRect = { mp_options.x, mp_options.y, 0, 0 };
	m_enabled = mp_options.enabled;
	m_metricsOnly = mp_options.metricsOnly;
	m_renderer = g_scene->getRenderer();

	updateFont();
	updateTexture();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
}

void RZUF3_TextRenderer::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);

	removeTexture();
	removeFont();

	m_renderer = nullptr;
}

void RZUF3_TextRenderer::setFontFilepath(std::string fontFilepath)
{
	removeTexture();
	removeFont();
	m_fontFilepath = fontFilepath;
	updateFont();
	updateTexture();

}

void RZUF3_TextRenderer::setDstPos(int x, int y)
{
	m_dstRect.x = x;
	m_dstRect.y = y;
}

void RZUF3_TextRenderer::setText(std::string text)
{
	m_text = text;
	removeTexture();
	updateTexture();
}

void RZUF3_TextRenderer::setStyle(RZUF3_TextStyle style)
{
	m_style = style;
	removeTexture();
	updateTexture();
}

void RZUF3_TextRenderer::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

RZUF3_TextStyle RZUF3_TextRenderer::getStyle()
{
	return m_style;
}

int RZUF3_TextRenderer::getWidth()
{
	return m_dstRect.w;
}

int RZUF3_TextRenderer::getHeight()
{
	return m_dstRect.h;
}

bool RZUF3_TextRenderer::getEnabled()
{
	return m_enabled;
}

void RZUF3_TextRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_object == nullptr) return;
	if (m_texture == nullptr) return;
	if (!m_enabled || m_metricsOnly) return;

	m_renderer->setAlign(m_style.alignment);
	m_renderer->drawTexture(
		m_object,
		m_texture,
		nullptr,
		m_dstRect
	);
}

void RZUF3_TextRenderer::removeFont()
{
	if (m_font == nullptr) return;

	TTF_CloseFont(m_font);
	m_font = nullptr;

	spdlog::info("Unloaded font: {}", this->m_fontFilepath);
}

void RZUF3_TextRenderer::updateFont()
{
	if (this->m_fontFilepath == "") return;

	this->m_font = TTF_OpenFont(this->m_fontFilepath.c_str(), 24);

	if (this->m_font == nullptr)
	{
		spdlog::error("Font {} not found", this->m_fontFilepath);
		return;
	}

	spdlog::info("Loaded font: {}", this->m_fontFilepath);
}

void RZUF3_TextRenderer::removeTexture()
{
	if (this->m_texture == nullptr) return;

	SDL_DestroyTexture(this->m_texture);
	this->m_texture = nullptr;
}

void RZUF3_TextRenderer::updateTexture()
{
	if (this->m_font == nullptr) return;

	TTF_SetFontStyle(m_font, m_style.style);
	TTF_SetFontSize(m_font, m_style.size);

	std::string text = m_style.useLangFile ? g_lang->getText(m_text) : m_text;
	if (text == "") {
		m_dstRect.w = 0;
		m_dstRect.h = 0;
		return;
	}

	if (m_metricsOnly) {
		int w, h;
		TTF_SizeUTF8(m_font, text.c_str(), &w, &h);
		m_dstRect.w = w;
		m_dstRect.h = h;
		return;
	}

	SDL_Surface* surface = TTF_RenderUTF8_LCD_Wrapped(
		this->m_font,
		text.c_str(),
		m_style.color, 
		m_style.bgColor, 
		m_style.wrapLength
	);

	if (surface == nullptr)
	{
		spdlog::error("Failed to render text: {}", TTF_GetError());
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->m_renderer->getSDLRenderer(), surface);
	/// temporarily commented out because it caused some weird exceptions in debug mode
	/// SDL_FreeSurface(surface);
	if (texture == nullptr)
	{
		spdlog::error("Failed to create text texture: {}", SDL_GetError());
		return;
	}

	this->m_texture = texture;

	SDL_QueryTexture(this->m_texture, nullptr, nullptr, &m_dstRect.w, &m_dstRect.h);
}