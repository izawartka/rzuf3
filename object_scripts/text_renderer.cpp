#include "text_renderer.h"
#include "../lang.h"

RZUF3_TextRenderer::RZUF3_TextRenderer(std::string fontFilepath, std::string text)
{
	mp_options.style.fontFilepath = fontFilepath;
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
	m_options = mp_options;
	m_origSize = { 0, 0, 0, 0 };
	m_renderer = g_scene->getRenderer();

	cacheLangFileText();
	updateFont();
	updateTexture();
	setUseOnDraw(m_options.useOnDraw);
}

void RZUF3_TextRenderer::deinit()
{
	setUseOnDraw(false);
	removeTexture();
	removeFont();

	m_renderer = nullptr;
}

void RZUF3_TextRenderer::setFontFilepath(std::string fontFilepath)
{
	RZUF3_TextStyle style = m_options.style;
	style.fontFilepath = fontFilepath;

	setStyle(style);
}

void RZUF3_TextRenderer::setDstPos(int x, int y)
{
	m_options.dstRect.x = x;
	m_options.dstRect.y = y;
}

void RZUF3_TextRenderer::setDstSize(int width, int height)
{
	m_options.dstRect.w = width;
	m_options.dstRect.h = height;
}

void RZUF3_TextRenderer::setOffset(int offsetX, int offsetY)
{
	m_options.cropRect.x = offsetX;
	m_options.cropRect.y = offsetY;
}

void RZUF3_TextRenderer::setMaxSize(int maxWidth, int maxHeight)
{
	m_options.cropRect.w = maxWidth;
	m_options.cropRect.h = maxHeight;
}

void RZUF3_TextRenderer::setText(std::string text)
{
	m_options.text = text;
	cacheLangFileText();
	removeTexture();
	updateTexture();
}

void RZUF3_TextRenderer::setStyle(RZUF3_TextStyle style)
{
	bool fontChanged = m_options.style.fontFilepath != style.fontFilepath;
	m_options.style = style;

	removeTexture();
	if (fontChanged) {
		removeFont();
		updateFont();
	}
	updateTexture();
}

void RZUF3_TextRenderer::setUseLangFile(bool useLangFile)
{
	m_options.useLangFile = useLangFile;

	cacheLangFileText();
	removeTexture();
	updateTexture();
}

void RZUF3_TextRenderer::setUseOnDraw(bool useOnDraw)
{
	m_options.useOnDraw = useOnDraw;

	if (useOnDraw && !m_options.metricsOnly && !hasOnDrawListener) {
		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_ADD_LISTENER(eventsManager, Draw);
		hasOnDrawListener = true;
	}

	if (!useOnDraw && hasOnDrawListener) {
		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_REMOVE_LISTENER(eventsManager, Draw);
		hasOnDrawListener = false;
	}
}

void RZUF3_TextRenderer::setWrapLength(int wrapLength)
{
	m_options.wrapLength = wrapLength;

	removeTexture();
	updateTexture();
}

void RZUF3_TextRenderer::setAlignment(RZUF3_Align alignment)
{
	m_options.alignment = alignment;
}

/* texture space x and y */
int RZUF3_TextRenderer::pointToCharIndex(int x, int y)
{
	if(m_font == nullptr) return -1;

	int lineHeight = TTF_FontHeight(m_font);

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

	int charIndex = 0;
	TTF_MeasureUTF8(m_font, targetLine.c_str(), x, nullptr, &charIndex);
	if(charIndex > targetLineLength) charIndex = targetLineLength;

	return targetLineStart + charIndex;
}

void RZUF3_TextRenderer::draw()
{
	if (m_options.metricsOnly) {
		spdlog::error("Cannot draw text with metricsOnly option enabled");
		return;
	}

	if (m_object == nullptr) return;
	if (m_texture == nullptr) return;

	SDL_Rect srcRect = m_origSize;
	if (srcRect.w > m_options.cropRect.w && m_options.cropRect.w != 0) srcRect.w = m_options.cropRect.w;
	if (srcRect.h > m_options.cropRect.h && m_options.cropRect.h != 0) srcRect.h = m_options.cropRect.h;
	srcRect.x = m_options.cropRect.x;
	srcRect.y = m_options.cropRect.y;

	if (srcRect.w + srcRect.x > m_origSize.w) srcRect.w = m_origSize.w - srcRect.x;
	if (srcRect.h + srcRect.y > m_origSize.h) srcRect.h = m_origSize.h - srcRect.y;

	SDL_Rect dstRect = m_options.dstRect;
	dstRect.w = dstRect.w > 0 ? dstRect.w : srcRect.w;
	dstRect.h = dstRect.h > 0 ? dstRect.h : srcRect.h;

	m_renderer->setAlign(m_options.alignment);
	m_renderer->drawTexture(
		m_object,
		m_texture,
		&srcRect,
		dstRect
	);
}

RZUF3_TextStyle RZUF3_TextRenderer::getStyle() const
{
	return m_options.style;
}

int RZUF3_TextRenderer::getWidth() const
{
	return m_origSize.w;
}

int RZUF3_TextRenderer::getHeight() const
{
	return m_origSize.h;
}

bool RZUF3_TextRenderer::getUseOnDraw() const
{
	return m_options.useOnDraw;
}

void RZUF3_TextRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (!m_options.useOnDraw || m_options.metricsOnly) return;
	draw();
}

void RZUF3_TextRenderer::removeFont()
{
	if (m_font == nullptr) return;

	TTF_CloseFont(m_font);
	m_font = nullptr;

	spdlog::info("Unloaded font: {}", this->m_options.style.fontFilepath);
}

void RZUF3_TextRenderer::updateFont()
{
	if (this->m_options.style.fontFilepath == "") {
		spdlog::error("Font filepath is empty");
		return;
	}

	this->m_font = TTF_OpenFont(this->m_options.style.fontFilepath.c_str(), 24);

	if (this->m_font == nullptr)
	{
		spdlog::error("Font {} not found", this->m_options.style.fontFilepath);
		return;
	}

	spdlog::info("Loaded font: {}", this->m_options.style.fontFilepath);
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

	TTF_SetFontStyle(m_font, m_options.style.style);
	TTF_SetFontSize(m_font, m_options.style.size);

	if (m_cachedText == "") {
		m_origSize.w = 0;
		m_origSize.h = 0;
		return;
	}

	if (m_options.metricsOnly) {
		int w, h;
		TTF_SizeUTF8(m_font, m_cachedText.c_str(), &w, &h);
		m_origSize.w = w;
		m_origSize.h = h;
		return;
	}

	SDL_Surface* surface = TTF_RenderUTF8_LCD_Wrapped(
		this->m_font,
		m_cachedText.c_str(),
		m_options.style.color, 
		m_options.style.bgColor, 
		m_options.wrapLength
	);

	if (surface == NULL)
	{
		spdlog::error("Failed to render text: {}", TTF_GetError());
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->m_renderer->getSDLRenderer(), surface);
	SDL_SetTextureBlendMode(texture, m_options.style.blendMode);

	SDL_FreeSurface(surface);

	if (texture == NULL)
	{
		spdlog::error("Failed to create text texture: {}", SDL_GetError());
		return;
	}

	this->m_texture = texture;

	SDL_QueryTexture(this->m_texture, nullptr, nullptr, &m_origSize.w, &m_origSize.h);
}

void RZUF3_TextRenderer::cacheLangFileText()
{
	std::string text = m_options.useLangFile ? g_lang->getText(m_options.text) : m_options.text;

	m_cachedText = text;
}
