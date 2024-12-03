#include "text_renderer.h"
#include "../assets/font.h"
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
	createFont();
	updateTexture();
	setUseOnDraw(m_options.useOnDraw);
}

void RZUF3_TextRenderer::deinit()
{
	setUseOnDraw(false);
	removeTexture();
	removeFont();

	m_renderer = nullptr;
	m_cachedText = "";
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
		createFont();
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

	if (useOnDraw && !m_options.metricsOnly && !m_hasOnDrawListener) {
		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_ADD_LISTENER(eventsManager, Draw);
		m_hasOnDrawListener = true;
	}

	if ((!useOnDraw || m_options.metricsOnly) && m_hasOnDrawListener) {
		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_REMOVE_LISTENER(eventsManager, Draw);
		m_hasOnDrawListener = false;
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

void RZUF3_TextRenderer::setMetricsOnly(bool metricsOnly)
{
	m_options.metricsOnly = metricsOnly;
	setUseOnDraw(m_options.useOnDraw);

	removeTexture();
	updateTexture();
}

/* texture space x and y */
/* does not work with wrapping */
int RZUF3_TextRenderer::pointToCharIndex(int x, int y)
{
	if(m_font == nullptr) return -1;

	int lineHeight = TTF_FontHeight(m_font);

	int targetLineIndex = y / lineHeight;
	if(y < 0) targetLineIndex = 0;
	if(y >= m_origSize.h) targetLineIndex = m_origSize.h / lineHeight - 1;

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

bool RZUF3_TextRenderer::charIndexToPoint(int index, int& x, int& y)
{
	if (m_font == nullptr) return false;
	if (index < 0 || index > m_cachedText.size()) return false;

	int lineHeight = TTF_FontHeight(m_font);

	if(m_cachedText.size() == 0)
	{
		x = 0;
		y = lineHeight;
		return true;
	}

	int nextLineIndex = 0;
	int targetLineStart = 0;
	int targetLineEnd = -1;
	bool endsWithNewline = m_cachedText.back() == '\n';

	while (targetLineEnd < index)
	{
		nextLineIndex++;
		targetLineStart = targetLineEnd + 1;
		targetLineEnd = m_cachedText.find('\n', targetLineStart);
		if (targetLineEnd == std::string::npos) targetLineEnd = m_cachedText.size();
	}

	int targetLineLength = targetLineEnd - targetLineStart;

	int charIndex = index - targetLineStart;

	std::string targetLineText = m_cachedText.substr(targetLineStart, targetLineLength) + "-";
	std::string targetText = targetLineText.substr(0, charIndex);

	TTF_SizeUTF8(m_font, targetText.c_str(), &x, nullptr);

	y = nextLineIndex * lineHeight;

	return true;
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

SDL_Texture* RZUF3_TextRenderer::getTexture() const
{
	return m_texture;
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

int RZUF3_TextRenderer::getFontAscent() const
{
	if (m_font == nullptr) return 0;

	return TTF_FontAscent(m_font);
}

int RZUF3_TextRenderer::getFontDescent() const
{
	if (m_font == nullptr) return 0;

	return TTF_FontDescent(m_font);
}

int RZUF3_TextRenderer::getFontHeight() const
{
	if (m_font == nullptr) return 0;

	return TTF_FontHeight(m_font);
}

void RZUF3_TextRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (!m_options.useOnDraw || m_options.metricsOnly) return;
	draw();
}

void RZUF3_TextRenderer::removeFont()
{
	m_font == nullptr;

	if (m_fontAsset != nullptr) g_scene->getAssetsManager()->removeAsset(m_fontAsset->getFilepath());
}

void RZUF3_TextRenderer::createFont()
{
	if (m_options.style.fontFilepath == "") {
		spdlog::error("TextRenderer: Font filepath is empty");
		return;
	}

	m_fontAsset = (RZUF3_Font*)(g_scene->getAssetsManager()->addAsset({ 
		m_options.style.fontFilepath, 
		RZUF3_Font::getInstance 
	}));

	m_font = m_fontAsset == nullptr ? nullptr : (TTF_Font*)m_fontAsset->getContent();
}

void RZUF3_TextRenderer::removeTexture()
{
	if (m_texture == nullptr) return;

	SDL_DestroyTexture(m_texture);
	m_texture = nullptr;
}

void RZUF3_TextRenderer::updateTexture()
{
	if (m_font == nullptr) return;

	TTF_SetFontSize(m_font, m_options.style.size);
	TTF_SetFontStyle(m_font, m_options.style.style);

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
		m_font,
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

	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer->getSDLRenderer(), surface);
	SDL_SetTextureBlendMode(texture, m_options.style.blendMode);

	SDL_FreeSurface(surface);

	if (texture == NULL)
	{
		spdlog::error("Failed to create text texture: {}", SDL_GetError());
		return;
	}

	m_texture = texture;

	SDL_QueryTexture(m_texture, nullptr, nullptr, &m_origSize.w, &m_origSize.h);
}

void RZUF3_TextRenderer::cacheLangFileText()
{
	m_cachedText = m_options.useLangFile ? g_lang->getText(m_options.text) : m_options.text;
}
