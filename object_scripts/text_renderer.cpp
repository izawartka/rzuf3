#include "text_renderer.h"

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
	m_font = nullptr;
	m_text = mp_options.text;
	m_style = mp_options.style;
	m_dstRect = { mp_options.x, mp_options.y, 0, 0 };
	m_renderer = this->getObject()->getScene()->getRenderer();
	m_texture = nullptr;

	updateFont();
	updateTexture();

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
}

void RZUF3_TextRenderer::deinit()
{
	removeTexture();
	removeFont();

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);
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

void RZUF3_TextRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_object == nullptr) return;
	if (m_texture == nullptr) return;

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

	SDL_Surface* surface = TTF_RenderText_LCD_Wrapped(
		this->m_font,
		this->m_text.c_str(),
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
	if (texture == nullptr)
	{
		spdlog::error("Failed to create text texture: {}", SDL_GetError());
		return;
	}

	SDL_FreeSurface(surface);
	this->m_texture = texture;

	SDL_QueryTexture(this->m_texture, nullptr, nullptr, &m_dstRect.w, &m_dstRect.h);
}