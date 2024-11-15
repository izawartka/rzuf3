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
	m_cropRect = mp_options.cropRect;
	m_origSize = { 0, 0, 0, 0 };
	m_enabled = mp_options.enabled;
	m_metricsOnly = mp_options.metricsOnly;
	m_renderer = g_scene->getRenderer();

	cacheLangFileText();
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

void RZUF3_TextRenderer::setMaxSize(int maxWidth, int maxHeight)
{
	m_cropRect.w = maxWidth;
	m_cropRect.h = maxHeight;
}

void RZUF3_TextRenderer::setOffset(int offsetX, int offsetY)
{
	m_cropRect.x = offsetX;
	m_cropRect.y = offsetY;
}

void RZUF3_TextRenderer::setText(std::string text)
{
	m_text = text;
	cacheLangFileText();
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

/* texture space x and y */
int RZUF3_TextRenderer::pointToCharIndex(int x, int y)
{
	if(m_font == nullptr) return -1;

	int lineCount = std::count(m_cachedText.begin(), m_cachedText.end(), '\n') + 1;
	int lineHeight = m_origSize.h / lineCount;

	int targetLineIndex = y / lineHeight;
	int targetLineStart = 0;
	int targetLineEnd = m_cachedText.size();

	for (int i = 0; i < targetLineIndex; i++)
	{
		targetLineStart = m_cachedText.find('\n', targetLineStart) + 1;
	}

	targetLineEnd = m_cachedText.find('\n', targetLineStart);
	if (targetLineEnd == std::string::npos) targetLineEnd = m_cachedText.size();

	int targetLineLength = targetLineEnd - targetLineStart;
	std::string targetLine = m_cachedText.substr(targetLineStart, targetLineLength) + " ";

	int targetLineWidth = 0;
	TTF_SizeUTF8(m_font, targetLine.c_str(), &targetLineWidth, nullptr);

	int estimatedIndex = (x * targetLineLength) / targetLineWidth;

	while (true) {
		if (estimatedIndex >= targetLineLength) {
			estimatedIndex = targetLineLength;
			break;
		}
		if (estimatedIndex < 0) {
			estimatedIndex = 0;
			break;
		}

		int guessWidth = 0;
		TTF_SizeUTF8(m_font, targetLine.substr(0, estimatedIndex).c_str(), &guessWidth, nullptr);

		if (guessWidth > x) {
			estimatedIndex--;
			continue;
		}

		int guessNextWidth = 0;
		TTF_SizeUTF8(m_font, targetLine.substr(0, estimatedIndex + 1).c_str(), &guessNextWidth, nullptr);

		if (guessWidth <= x && guessNextWidth > x) break;

		estimatedIndex++;
	}

	return targetLineStart + estimatedIndex;
}

RZUF3_TextStyle RZUF3_TextRenderer::getStyle() const
{
	return m_style;
}

int RZUF3_TextRenderer::getWidth() const
{
	return m_origSize.w;
}

int RZUF3_TextRenderer::getHeight() const
{
	return m_origSize.h;
}

bool RZUF3_TextRenderer::getEnabled() const
{
	return m_enabled;
}

void RZUF3_TextRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_object == nullptr) return;
	if (m_texture == nullptr) return;
	if (!m_enabled || m_metricsOnly) return;

	SDL_Rect srcRect = m_origSize;
	if(srcRect.w > m_cropRect.w && m_cropRect.w != 0) srcRect.w = m_cropRect.w;
	if(srcRect.h > m_cropRect.h && m_cropRect.h != 0) srcRect.h = m_cropRect.h;
	srcRect.x = m_cropRect.x;
	srcRect.y = m_cropRect.y;

	if(srcRect.w + srcRect.x > m_origSize.w) srcRect.w = m_origSize.w - srcRect.x;
	if(srcRect.h + srcRect.y > m_origSize.h) srcRect.h = m_origSize.h - srcRect.y;

	SDL_Rect dstRect = m_dstRect;
	dstRect.w = srcRect.w;
	dstRect.h = srcRect.h;

	m_renderer->setAlign(m_style.alignment);
	m_renderer->drawTexture(
		m_object,
		m_texture,
		&srcRect,
		dstRect
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

	if (m_cachedText == "") {
		m_origSize.w = 0;
		m_origSize.h = 0;
		return;
	}

	if (m_metricsOnly) {
		int w, h;
		TTF_SizeUTF8(m_font, m_cachedText.c_str(), &w, &h);
		m_origSize.w = w;
		m_origSize.h = h;
		return;
	}

	SDL_Surface* surface = TTF_RenderUTF8_LCD_Wrapped(
		this->m_font,
		m_cachedText.c_str(),
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

	SDL_QueryTexture(this->m_texture, nullptr, nullptr, &m_origSize.w, &m_origSize.h);
}

void RZUF3_TextRenderer::cacheLangFileText()
{
	std::string text = m_style.useLangFile ? g_lang->getText(m_text) : m_text;
	if (text == "") return;

	m_cachedText = text;
}
