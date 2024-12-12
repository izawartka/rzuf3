#include "text_sel_renderer.h"
#include "../lang.h"
#include "../game.h"

RZUF3_TextSelRenderer::RZUF3_TextSelRenderer(std::string fontFilepath, std::string text)
{
	mp_options.style.textStyle.fontFilepath = fontFilepath;
	mp_options.text = text;
}

RZUF3_TextSelRenderer::RZUF3_TextSelRenderer(RZUF3_TextSelRendererOptions options)
{
	mp_options = options;
}

RZUF3_TextSelRenderer::~RZUF3_TextSelRenderer()
{
}

void RZUF3_TextSelRenderer::init()
{
	m_options = mp_options;
	cacheLangFileText();
	m_selStart = m_cachedText.size();
	m_selEnd = m_cachedText.size();
	createTextRenderers();
	updateSelRects();
	createCombinedTexture();
	cacheRealRects();

	setUseLangFile(m_options.useLangFile);
	setUseOnDraw(m_options.useOnDraw);
	setUseKeyboardEvents(m_options.useKeyboardEvents);
	setUseMouseEvents(m_options.useMouseEvents);

	_ADD_LISTENER(g_scene->getEventsManager(), TextSelFocus);
}

void RZUF3_TextSelRenderer::deinit()
{
	_REMOVE_LISTENER(g_scene->getEventsManager(), TextSelFocus);

	m_options.useLangFile = false;
	setUseLangFile(false);
	setUseOnDraw(false);
	setUseKeyboardEvents(false);
	setUseMouseEvents(false);

	removeTextRenderers();
	removeCombinedTexture();

	m_selStart = 0;
	m_selEnd = 0;
	m_isFocused = false;
	m_cachedText = "";
	m_selRects.clear();
}

void RZUF3_TextSelRenderer::setFontFilepath(std::string fontFilepath)
{
	RZUF3_TextSelStyle style = m_options.style;
	style.textStyle.fontFilepath = fontFilepath;

	setStyle(style);
}

void RZUF3_TextSelRenderer::setDstPos(int x, int y)
{
	m_options.dstRect.x = x;
	m_options.dstRect.y = y;

	cacheRealRects();
}

void RZUF3_TextSelRenderer::setDstSize(int width, int height)
{
	m_options.dstRect.w = width;
	m_options.dstRect.h = height;

	cacheRealRects();
}

void RZUF3_TextSelRenderer::setOffset(int offsetX, int offsetY)
{
	m_options.cropRect.x = offsetX;
	m_options.cropRect.y = offsetY;

	cacheRealRects();
}

void RZUF3_TextSelRenderer::setMaxSize(int maxWidth, int maxHeight)
{
	m_options.cropRect.w = maxWidth;
	m_options.cropRect.h = maxHeight;

	cacheRealRects();
}

void RZUF3_TextSelRenderer::setText(std::string text, bool resetSelection)
{
	m_options.text = text;
	cacheLangFileText();
	m_textRenderer->setText(m_cachedText);
	m_textRendererSelected->setText(m_cachedText);

	if (resetSelection) {
		m_selStart = m_cachedText.size();
		m_selEnd = m_cachedText.size();
		m_isSelecting = false;
	}

	if(m_selStart > m_cachedText.size()) m_selStart = m_cachedText.size();
	if(m_selEnd > m_cachedText.size()) m_selEnd = m_cachedText.size();

	updateSelRects();
	createCombinedTexture();
	cacheRealRects();
	sendSelectionChangeEvent();
}

void RZUF3_TextSelRenderer::setStyle(RZUF3_TextSelStyle style)
{
	if (!m_textRenderer || !m_textRendererSelected) return;

	m_options.style = style;
	m_textRenderer->setStyle(style.textStyle);

	style.textStyle.color = style.textSelColor;
	style.textStyle.bgColor = style.textSelBgColor;

	m_textRendererSelected->setStyle(style.textStyle);

	updateSelRects();
	createCombinedTexture();
	cacheRealRects();
}

void RZUF3_TextSelRenderer::setUseLangFile(bool useLangFile)
{
	bool wasTheSame = m_options.useLangFile == useLangFile;
	m_options.useLangFile = useLangFile;
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();

	if (useLangFile && !m_hasOnLangChangeListener) {
		_ADD_LISTENER(eventsManager, LangChange);
		m_hasOnLangChangeListener = true;
	}
	else if (!useLangFile && m_hasOnLangChangeListener) {
		_REMOVE_LISTENER(eventsManager, LangChange);
		m_hasOnLangChangeListener = false;
	}

	if(!wasTheSame) setText(m_options.text, true);
}

void RZUF3_TextSelRenderer::setUseOnDraw(bool useOnDraw)
{
	m_options.useOnDraw = useOnDraw;

	if (useOnDraw && !m_hasOnDrawListener) {
		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_ADD_LISTENER(eventsManager, Draw);
		m_hasOnDrawListener = true;
	}

	if (!useOnDraw && m_hasOnDrawListener) {
		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_REMOVE_LISTENER(eventsManager, Draw);
		m_hasOnDrawListener = false;
	}
}

void RZUF3_TextSelRenderer::setUseMouseEvents(bool useMouseEvents)
{
	m_options.useMouseEvents = useMouseEvents;

	if (useMouseEvents && !m_hasMouseEventsListener) {
		RZUF3_ClickableOptions clickableOptions;
		clickableOptions.useOnHoverCursor = m_options.useOnHoverCursor;
		clickableOptions.onHoverCursorId = SDL_SYSTEM_CURSOR_IBEAM;
		clickableOptions.useOnSetRect = false;
		clickableOptions.rect = m_realDstRect;
		clickableOptions.alignment = m_options.alignment;
		clickableOptions.insideOnly = false;
		clickableOptions.blockUnderneath = false;

		m_clickable = new RZUF3_Clickable(clickableOptions);
		m_object->addScript(m_clickable);

		RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
		_ADD_LISTENER(objEventsManager, MouseDown);
		_ADD_LISTENER(objEventsManager, MouseMove);

		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_ADD_LISTENER(eventsManager, MouseUp);

		m_hasMouseEventsListener = true;
	}

	if (!useMouseEvents && m_hasMouseEventsListener) {
		RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
		_REMOVE_LISTENER(objEventsManager, MouseDown);
		_REMOVE_LISTENER(objEventsManager, MouseMove);

		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_REMOVE_LISTENER(eventsManager, MouseUp);

		m_object->removeScript(m_clickable);
		delete m_clickable;
		m_clickable = nullptr;

		m_hasMouseEventsListener = false;
		m_isSelecting = false;
	}
}

void RZUF3_TextSelRenderer::setUseKeyboardEvents(bool useKeyboardEvents)
{
	m_options.useKeyboardEvents = useKeyboardEvents;

	if (useKeyboardEvents && !m_hasKeyboardEventsListener) {
		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_ADD_LISTENER(eventsManager, KeyDown);
		_ADD_LISTENER(eventsManager, KeyUp);

		m_hasKeyboardEventsListener = true;
	}

	if (!useKeyboardEvents && m_hasKeyboardEventsListener) {
		RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
		_REMOVE_LISTENER(eventsManager, KeyDown);
		_REMOVE_LISTENER(eventsManager, KeyUp);

		m_hasKeyboardEventsListener = false;
		m_isShiftDown = false;
		m_isCtrlDown = false;
	}
}

void RZUF3_TextSelRenderer::setWrapLength(int wrapLength)
{
	m_options.wrapLength = wrapLength;
	m_textRenderer->setWrapLength(wrapLength);
	m_textRendererSelected->setWrapLength(wrapLength);

	updateSelRects();
	createCombinedTexture();
	cacheRealRects();
}

void RZUF3_TextSelRenderer::setAlignment(RZUF3_Align alignment)
{
	m_options.alignment = alignment;

	if(m_textRenderer != nullptr) m_textRenderer->setAlignment(alignment);
}

void RZUF3_TextSelRenderer::setForceFocus(bool forceFocus)
{
	m_options.forceFocus = forceFocus;
}

void RZUF3_TextSelRenderer::setFocused(bool focused) const
{
	if(focused == m_isFocused) return;

	std::string id = focused ? m_object->getName() : "";
	RZUF3_TextSelFocusEvent event(id);
	g_scene->getEventsManager()->dispatchEvent(&event);
}

void RZUF3_TextSelRenderer::setSelection(int start, int end)
{
	if(start < 0) start = 0;
	if(end < 0) end = 0;
	if(start > (int)m_cachedText.size()) start = m_cachedText.size();
	if(end > (int)m_cachedText.size()) end = m_cachedText.size();

	m_selStart = start;
	m_selEnd = end;
	m_isSelecting = false;

	updateSelRects();
	createCombinedTexture();
	sendSelectionChangeEvent();
}

void RZUF3_TextSelRenderer::setSelectionEndFromXY(int x, int y, bool alsoStart)
{
	if (m_textRenderer == nullptr) return;

	int charIndex = pointToCharIndex(x, y);
	if (charIndex == -1) return;

	if(alsoStart) m_selStart = charIndex;
	m_selEnd = charIndex;

	updateSelRects();
	createCombinedTexture();
	sendSelectionChangeEvent();
}

int RZUF3_TextSelRenderer::pointToCharIndex(int x, int y)
{
	return m_textRenderer->pointToCharIndex(x, y);
}

bool RZUF3_TextSelRenderer::charIndexToPoint(int index, int& x, int& y)
{
	return m_textRenderer->charIndexToPoint(index, x, y);
}

void RZUF3_TextSelRenderer::draw()
{
	if (m_object == nullptr) return;
	if (m_combinedTexture == nullptr) return;

	g_renderer->setAlign(m_options.alignment);
	g_renderer->drawTexture(
		m_object,
		m_combinedTexture,
		&m_realSrcRect,
		m_realDstRect
	);
}

SDL_Texture* RZUF3_TextSelRenderer::getTexture() const
{
	return m_combinedTexture;
}

RZUF3_TextSelStyle RZUF3_TextSelRenderer::getStyle() const
{
	return m_options.style;
}

int RZUF3_TextSelRenderer::getWidth() const
{
	return m_origSize.w;
}

int RZUF3_TextSelRenderer::getHeight() const
{
	return m_origSize.h;
}

bool RZUF3_TextSelRenderer::getUseOnDraw() const
{
	return m_options.useOnDraw;
}

bool RZUF3_TextSelRenderer::getUseMouseEvents() const
{
	return m_options.useMouseEvents;
}

bool RZUF3_TextSelRenderer::getUseKeyboardEvents() const
{
	return m_options.useKeyboardEvents;
}

int RZUF3_TextSelRenderer::getFontAscent() const
{
	if(m_textRenderer == nullptr) return 0;
	return m_textRenderer->getFontAscent();
}

int RZUF3_TextSelRenderer::getFontDescent() const
{
	if(m_textRenderer == nullptr) return 0;
	return m_textRenderer->getFontDescent();
}

int RZUF3_TextSelRenderer::getFontHeight() const
{
	if(m_textRenderer == nullptr) return 0;
	return m_textRenderer->getFontHeight();
}

void RZUF3_TextSelRenderer::getSelection(int& start, int& end) const
{
	start = m_selStart;
	end = m_selEnd;
}

int RZUF3_TextSelRenderer::getSelectionStart() const
{
	return m_selStart;
}

int RZUF3_TextSelRenderer::getSelectionEnd() const
{
	return m_selEnd;
}

std::string RZUF3_TextSelRenderer::getSelectedText() const
{
	if(m_selStart == m_selEnd) return "";

	int start = m_selStart;
	int end = m_selEnd;
	if (start > end) std::swap(start, end);

	return m_cachedText.substr(start, end - start);
}

void RZUF3_TextSelRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_options.useOnDraw) draw();
}

void RZUF3_TextSelRenderer::onLangChange(RZUF3_LangChangeEvent* event)
{
	if(!m_options.useLangFile) return;

	setText(m_cachedText, true);
}

void RZUF3_TextSelRenderer::onTextSelFocus(RZUF3_TextSelFocusEvent* event)
{
	m_isFocused = event->getId() == m_object->getName();

	if (!m_isFocused) {
		setSelection(m_selEnd, m_selEnd);
		m_isSelecting = false;
	}
}

void RZUF3_TextSelRenderer::onKeyDown(RZUF3_KeyDownEvent* event)
{
	if (!m_options.useKeyboardEvents) return;
	if (!m_isFocused && !m_options.forceFocus) return;
	int textSize = m_cachedText.size();
	int target = -1;

	int realStart = std::min(m_selStart, m_selEnd);
	int realEnd = std::max(m_selStart, m_selEnd);

	switch (event->getKeyCode()) {
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			m_isShiftDown = true;
			break;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			m_isCtrlDown = true;
			break;
		case SDLK_LEFT:
			if (m_isCtrlDown) break;
			target = m_selEnd - 1;
			if (m_selEnd != m_selStart && !m_isShiftDown) target = realStart;
			if(target < 0) target = 0;
			/// todo: ctrl + left
			break;
		case SDLK_RIGHT:
			if (m_isCtrlDown) break;
			target = m_selEnd + 1;
			if (m_selEnd != m_selStart && !m_isShiftDown) target = realEnd;
			if(target > textSize) target = textSize;
			/// todo: ctrl + right
			break;
		case SDLK_HOME:
			target = 0;
			if (!m_isCtrlDown && m_selEnd > 0) {
				target = m_cachedText.find_last_of("\n", m_selEnd - 1);
				if (target == std::string::npos) target = 0;
			}
			break;
		case SDLK_END:
			target = textSize;
			if (!m_isCtrlDown && m_selEnd < textSize) {
				target = m_cachedText.find_first_of("\n", m_selEnd + 1);
				if (target == std::string::npos) target = textSize;
			}
			break;
		case SDLK_UP:
			if (m_isCtrlDown) break;
			target = getNeighbourCharIndexVertically(m_selEnd, true);
			break;
		case SDLK_DOWN:
			if (m_isCtrlDown) break;
			target = getNeighbourCharIndexVertically(m_selEnd, false);
			break;
		case SDLK_a:
			if (m_isCtrlDown) setSelection(0, textSize);
			break;
		case SDLK_x:
			if(m_options.preventCtrlX) break;
		case SDLK_c:
			if (m_isCtrlDown) {
				g_game->copyToClipboard(getSelectedText());
			}
			break;
	}

	if (target != -1) {
		if (m_isShiftDown) setSelection(m_selStart, target);
		else setSelection(target, target);
	}
}

void RZUF3_TextSelRenderer::onKeyUp(RZUF3_KeyUpEvent* event)
{
	if (!m_options.useKeyboardEvents) return;

	switch (event->getKeyCode()) {
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			m_isShiftDown = false;
			break;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			m_isCtrlDown = false;
			break;
	}
}

void RZUF3_TextSelRenderer::onMouseUp(RZUF3_MouseUpEvent* event)
{
	if (!m_options.useMouseEvents) return;
	if (event->getButton() != SDL_BUTTON_LEFT) return;

	m_isSelecting = false;
}

void RZUF3_TextSelRenderer::onMouseDown(RZUF3_MouseDownEvent* event)
{
	if (!m_options.useMouseEvents) return;
	if (event->getButton() != SDL_BUTTON_LEFT) return;

	int x = event->getX();
	int y = event->getY();

	if((x < 0 || x > m_realDstRect.w || y < 0 || y > m_realDstRect.h) && !m_options.forceFocus) {
		setFocused(false);
		return;
	}

	x += m_options.cropRect.x;
	y += m_options.cropRect.y;

	setFocused(true);
	m_isSelecting = true;
	setSelectionEndFromXY(x, y, !m_isShiftDown);
}

void RZUF3_TextSelRenderer::onMouseMove(RZUF3_MouseMoveEvent* event)
{
	if (!m_options.useMouseEvents) return;
	if(!m_isSelecting) return;

	int x = event->getX() + m_options.cropRect.x;
	int y = event->getY() + m_options.cropRect.y;

	setSelectionEndFromXY(x, y, false);
}

int RZUF3_TextSelRenderer::getNeighbourCharIndexVertically(int index, bool up)
{
	/// TODO: improve
	int x, y;
	if (!charIndexToPoint(index, x, y)) return -1;

	int lineHeight = m_textRenderer->getFontHeight();
	int newX = x + 1;
	int newY = y - 1 + (up ? -lineHeight : lineHeight);

	if(newY < 0 || newY >= m_origSize.h) return -1;

	return pointToCharIndex(newX, newY);
}

RZUF3_TextRendererStyle RZUF3_TextSelRenderer::getSelectedTextStyle() const
{
	RZUF3_TextRendererStyle style = m_options.style.textStyle;
	style.color = m_options.style.textSelColor;
	style.bgColor = m_options.style.textSelBgColor;

	return style;
}

void RZUF3_TextSelRenderer::removeTextRenderers()
{
	if (m_textRenderer != nullptr) {
		m_object->removeScript(m_textRenderer);
		delete m_textRenderer;
		m_textRenderer = nullptr;
	}

	if (m_textRendererSelected != nullptr) {
		m_object->removeScript(m_textRendererSelected);
		delete m_textRendererSelected;
		m_textRendererSelected = nullptr;
	}
}

void RZUF3_TextSelRenderer::createTextRenderers()
{
	if (m_textRenderer != nullptr || m_textRendererSelected != nullptr) return;

	RZUF3_TextRendererOptions options;
	options.text = m_cachedText;
	options.metricsOnly = false;
	options.useOnDraw = false;
	options.useLangFile = false;
	options.alignment = RZUF3_Align_TopLeft;
	options.dstRect = { 0, 0, 0, 0 };
	options.cropRect = { 0, 0, 0, 0 };

	options.wrapLength = m_options.wrapLength;
	options.style = m_options.style.textStyle;
	m_textRenderer = new RZUF3_TextRenderer(options);
	if (!m_textRenderer) {
		spdlog::error("TextSelRenderer: Failed to create original text renderer");
		return;
	}
	m_object->addScript(m_textRenderer);

	options.style = getSelectedTextStyle();
	m_textRendererSelected = new RZUF3_TextRenderer(options);
	if (!m_textRendererSelected) {
		spdlog::error("TextSelRenderer: Failed to create selected text renderer");
		return;
	}
	m_object->addScript(m_textRendererSelected);
}

void RZUF3_TextSelRenderer::removeCombinedTexture()
{
	if (m_combinedTexture != nullptr) {
		SDL_DestroyTexture(m_combinedTexture);
		m_combinedTexture = nullptr;
	}

	m_origSize = { 0, 0, 0, 0 };
	m_realDstRect = { 0, 0, 0, 0 };
	m_realSrcRect = { 0, 0, 0, 0 };
}

void RZUF3_TextSelRenderer::createCombinedTexture()
{
	if(m_textRenderer == nullptr || m_textRendererSelected == nullptr) return;
	if(m_combinedTexture != nullptr) SDL_DestroyTexture(m_combinedTexture);
	m_combinedTexture = nullptr;

	m_origSize.w = m_textRenderer->getWidth();
	m_origSize.h = m_textRenderer->getHeight();

	if(m_origSize.w == 0 || m_origSize.h == 0) return;

	SDL_Texture* tempTexture = SDL_CreateTexture(
		g_renderer->getSDLRenderer(),
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		m_origSize.w,
		m_origSize.h
	);

	SDL_Texture* prevTarget = SDL_GetRenderTarget(g_renderer->getSDLRenderer());
	SDL_SetRenderTarget(g_renderer->getSDLRenderer(), tempTexture);
	bool prevUseObjectPos = g_renderer->getUseObjectPos();
	g_renderer->setUseObjectPos(false);

	m_textRenderer->draw();

	SDL_Texture* selTexture = m_textRendererSelected->getTexture();
	for (SDL_Rect rect : m_selRects) {
		if(rect.x + rect.w > m_origSize.w) rect.w = m_origSize.w - rect.x;
		if(rect.y + rect.h > m_origSize.h) rect.h = m_origSize.h - rect.y;
		SDL_RenderCopy(g_renderer->getSDLRenderer(), selTexture, &rect, &rect);
	}

	if (!g_renderer->createStaticTexture(m_combinedTexture, m_origSize.w, m_origSize.h)) {
		spdlog::error("TextSelRenderer: Failed to create combined texture");
	}

	SDL_SetRenderTarget(g_renderer->getSDLRenderer(), prevTarget);
	SDL_DestroyTexture(tempTexture);
	g_renderer->setUseObjectPos(prevUseObjectPos);
}

void RZUF3_TextSelRenderer::cacheLangFileText()
{
	m_cachedText = m_options.useLangFile ? g_lang->getText(m_options.text) : m_options.text;
}

void RZUF3_TextSelRenderer::cacheRealRects()
{
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

	m_realSrcRect = srcRect;
	m_realDstRect = dstRect;

	if (m_clickable != nullptr) m_clickable->setRect(dstRect);
}

void RZUF3_TextSelRenderer::updateSelRects()
{
	m_selRects.clear();
	if(m_textRenderer == nullptr) return;
	if(m_selStart == m_selEnd) return;

	int selStart = m_selStart;
	int selEnd = m_selEnd;
	if (selStart > selEnd) std::swap(selStart, selEnd);
	std::string selText = m_cachedText.substr(selStart, selEnd - selStart);

	int x1, y1, x2, y2;
	m_textRenderer->charIndexToPoint(selStart, x1, y1);
	m_textRenderer->charIndexToPoint(selEnd, x2, y2);
	int lineHeight = m_textRenderer->getFontHeight();

	// if no newlines in selection
	if (y1 == y2) {
		y1 -= lineHeight;

		SDL_Rect rect = { x1, y1, x2 - x1, y2 - y1 };
		m_selRects.push_back(rect);
		return;
	}

	// if newlines in selection
	int textWidth = m_textRenderer->getWidth();

	int currentLineEndCharIndex = selStart;
	for (int y = y1 - lineHeight; y < y2; y += lineHeight) {
		currentLineEndCharIndex = m_cachedText.find('\n', currentLineEndCharIndex);

		int xStart = 0;
		if (y == y1 - lineHeight) xStart = x1;

		int xEnd = 0;
		if(y + lineHeight >= y2) xEnd = x2;
		else {
			int tempY;
			m_textRenderer->charIndexToPoint(currentLineEndCharIndex, xEnd, tempY);
			currentLineEndCharIndex++;
		}

		SDL_Rect rect = { xStart, y, xEnd - xStart, lineHeight };
		m_selRects.push_back(rect);
	}
}

void RZUF3_TextSelRenderer::sendSelectionChangeEvent()
{
	RZUF3_TextSelectionChangeEvent event(m_selStart, m_selEnd);
	m_object->getEventsManager()->dispatchEvent(&event);
}