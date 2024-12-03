#include "text_input.h"
#include "../game.h"

RZUF3_TextInput::RZUF3_TextInput(RZUF3_TextInputOptions options)
{
	mp_options = options;
}

RZUF3_TextInput::~RZUF3_TextInput()
{

}

void RZUF3_TextInput::init()
{
	m_options = mp_options;
	setText(mp_options.text);

	m_renderer = g_scene->getRenderer();

	m_clickable = new RZUF3_Clickable();
	m_object->addScript(m_clickable);
	m_clickable->setOnHoverCursor(SDL_SYSTEM_CURSOR_IBEAM);

	m_blinkTimer = new RZUF3_Timer("blinkTimer", 0);
	m_object->addScript(m_blinkTimer);

	createTextRenderer();
	updateStyle();

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, MouseDown);
	_ADD_LISTENER(objEventsManager, MouseDownOutside);
	_ADD_LISTENER(objEventsManager, UISetStringValue);
	_ADD_LISTENER(objEventsManager, SetRect);
	_ADD_LISTENER(objEventsManager, Timer);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
	_ADD_LISTENER(eventsManager, TextInputFocus);
	_ADD_LISTENER(eventsManager, TextInput);
	_ADD_LISTENER(eventsManager, KeyDown);
}

void RZUF3_TextInput::deinit()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, MouseDown);
	_REMOVE_LISTENER(objEventsManager, MouseDownOutside);
	_REMOVE_LISTENER(objEventsManager, UISetStringValue);
	_REMOVE_LISTENER(objEventsManager, SetRect);
	_REMOVE_LISTENER(objEventsManager, Timer);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);
	_REMOVE_LISTENER(eventsManager, TextInputFocus);
	_REMOVE_LISTENER(eventsManager, TextInput);
	_REMOVE_LISTENER(eventsManager, KeyDown);

	removeTextRenderer();

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;

	m_object->removeScript(m_blinkTimer);
	delete m_blinkTimer;
	m_blinkTimer = nullptr;

	m_renderer = nullptr;
}

void RZUF3_TextInput::setMaxChars(int maxChars)
{
	m_options.maxChars = maxChars;

	controlledSetText(m_text);
}

void RZUF3_TextInput::setStyle(RZUF3_TextInputStyle style)
{
	m_options.style = style;

	updateStyle();
	updateTextRenderer();
	updateCursorRectAndScroll();
}

void RZUF3_TextInput::setFocusedStyle(RZUF3_TextInputStyle style)
{
	m_options.focusedStyle = style;

	updateStyle();
	updateTextRenderer();
	updateCursorRectAndScroll();
}

void RZUF3_TextInput::setText(std::string text)
{
	controlledSetText(text);

	int end = m_text.size();
	setCursorPos(end);
}

void RZUF3_TextInput::setMultiline(bool multiline)
{
	m_options.multiline = multiline;

	controlledSetText(m_text);
}

void RZUF3_TextInput::setFocused(bool focused)
{
	if(focused == m_isFocused) return;

	std::string id = m_object->getName();
	SDL_Rect rect = getBorderRect(true);
	g_renderer->objectToScreenRect(m_object, rect);

	if (focused) {
		g_game->requestTextInput(id, rect);
	}
	else {
		g_game->stopTextInput(id);
	}
}

void RZUF3_TextInput::setCursorPos(int pos)
{
	if(!m_textRenderer) return;

	m_textRenderer->setSelection(pos, pos);
	updateCursorRectAndScroll();
}

void RZUF3_TextInput::setCursorPosFromXY(int x, int y)
{
	if (m_textRenderer == nullptr) return;

	RZUF3_TextInputStyle* style = getStyle(m_isFocused);

	x += m_scrollX - style->horizontalPadding;
	y += m_scrollY - style->verticalPadding;

	if(y < 0) y = 0;
	if(y >= m_textRenderer->getHeight()) y = m_textRenderer->getHeight() - 1;

	int charIndex = m_textRenderer->pointToCharIndex(x, y);
	if(charIndex > -1) setCursorPos(charIndex);
}

void RZUF3_TextInput::addText(std::string text, bool noNewLineCheck)
{
	int start, end;
	m_textRenderer->getSelection(start, end);
	if(start > end) std::swap(start, end);

	std::string newText = m_text;

	if(start != end) {
		newText = newText.erase(start, start - end);
	}
	newText = newText.insert(start, text);
	int newCursorPos = start + text.size();

	controlledSetText(newText, noNewLineCheck);
	setCursorPos(newCursorPos);
}

void RZUF3_TextInput::removeText(bool backspace)
{
	int start, end;
	m_textRenderer->getSelection(start, end);
	if(start > end) std::swap(start, end);

	std::string newText = m_text;
	int newCursorPos = start;

	if(start != end) {
		newText = newText.erase(start, end - start);
	} else if(backspace && start > 0) {
		newText = newText.erase(start - 1, 1);
		newCursorPos = start - 1;
	}
	else if(!backspace && end < newText.size()) {
		newText = newText.erase(start, 1);
	}

	controlledSetText(newText, true);
	setCursorPos(newCursorPos);
}

RZUF3_TextInputStyle* RZUF3_TextInput::getStyle(bool focused)
{
	return focused ? &m_options.focusedStyle : &m_options.style;
}

std::string RZUF3_TextInput::getText() const
{
	return m_text;
}

bool RZUF3_TextInput::getMultiline() const
{
	return m_options.multiline;
}

bool RZUF3_TextInput::getFocused() const
{
	return m_isFocused;
}

SDL_Rect RZUF3_TextInput::getBorderRect(bool focused)
{
	if (m_textRenderer == nullptr) return { 0, 0, 0, 0 };

	RZUF3_TextInputStyle* style = getStyle(focused);

	int textWidth = m_textRenderer->getWidth();
	int textHeight = m_textRenderer->getHeight();
	if(m_text.size() == 0 || m_text.find_last_of("\n") == m_text.size() - 1) {
		textHeight += m_textRenderer->getFontHeight();
	}

	return {
		style->rect.x,
		style->rect.y,
		style->rect.w ? style->rect.w : textWidth + style->horizontalPadding * 2,
		style->rect.h ? style->rect.h : textHeight + style->verticalPadding * 2
	};
}

void RZUF3_TextInput::onMouseDown(RZUF3_MouseDownEvent* event)
{
	setFocused(true);

	int x = event->getX();
	int y = event->getY();

	setCursorPosFromXY(x, y);
}

void RZUF3_TextInput::onMouseDownOutside(RZUF3_MouseDownOutsideEvent* event)
{
	if (!m_isFocused) return;

	setFocused(false);
}

void RZUF3_TextInput::onUISetStringValue(RZUF3_UISetStringValueEvent* event)
{
	std::string text = event->getValue();

	setText(text);
}

/* updates rect for both focused and unfocused style */
void RZUF3_TextInput::onSetRect(RZUF3_SetRectEvent* event)
{
	SDL_Rect rect = event->getRect();

	m_options.style.rect = rect;
	m_options.focusedStyle.rect = rect;

	updateStyle();
	updateCursorRectAndScroll();
}

void RZUF3_TextInput::onTimer(RZUF3_TimerEvent* event)
{
	if(event->getId() != "blinkTimer") return;

	m_isBlinking = !m_isBlinking;
}

void RZUF3_TextInput::onDraw(RZUF3_DrawEvent* event)
{
	RZUF3_Object* obj = m_object;
	RZUF3_TextInputStyle* style = getStyle(m_isFocused);
	SDL_Rect borderRect = getBorderRect(m_isFocused);

	m_renderer->setAlign(RZUF3_Align_TopLeft);
	m_renderer->setColor(style->bgColor);
	m_renderer->fillRect(obj, borderRect);
	m_renderer->setColor(style->borderColor);
	m_renderer->drawRect(obj, borderRect, style->borderWidth);

	if (m_textRenderer) m_textRenderer->draw();

	if (m_isBlinking && m_isFocused) {
		m_renderer->setColor(style->blinkColor);
		m_renderer->setAlign(RZUF3_Align_TopLeft);
		m_renderer->fillRect(obj, m_cachedCursorRect);
	}
}

void RZUF3_TextInput::onTextInputFocus(RZUF3_TextInputFocusEvent* event)
{
	std::string id = event->getId();
	m_isFocused = id == m_object->getName();

	if (m_textRenderer != nullptr) {
		m_textRenderer->setFocused(m_isFocused);
		m_textRenderer->setForceFocus(m_isFocused);
	}

	updateStyle();
	updateCursorRectAndScroll();
}

void RZUF3_TextInput::onTextSelectionChange(RZUF3_TextSelectionChangeEvent* event)
{
	updateCursorRectAndScroll();
}

void RZUF3_TextInput::onTextInput(RZUF3_TextInputEvent* event)
{
	if (!m_isFocused) return;
	std::string addedText = event->getText();
	if (addedText.size() > 1) return; // temporary fix for non-ascii characters lol

	addText(addedText);
}

void RZUF3_TextInput::onKeyDown(RZUF3_KeyDownEvent* event)
{
	if (!m_isFocused) return;

	std::string text = m_text;
	switch (event->getKeyCode()) {
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			m_isCtrlDown = true;
			break;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			m_isShiftDown = true;
			break;
		case SDLK_BACKSPACE:
			removeText(true);
			break;
		case SDLK_DELETE:
			removeText(false);
			break;
		case SDLK_RETURN:
			if (m_options.multiline) addText("\n", true);
			break;
		case SDLK_x:
			if(!m_isCtrlDown) break;
			if(!m_textRenderer) break;
			if(m_textRenderer->getSelectionStart() == m_textRenderer->getSelectionEnd()) break;
			g_game->copyToClipboard(m_textRenderer->getSelectedText());
			removeText(false);
			break;
		case SDLK_v:
			if(!m_isCtrlDown) break;
			addText(g_game->getClipboardText());
			break;
		default:
			return;
	}
}

void RZUF3_TextInput::onKeyUp(RZUF3_KeyUpEvent* event)
{
	switch (event->getKeyCode()) {
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			m_isCtrlDown = false;
			break;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			m_isShiftDown = false;
			break;
		default:
			return;
	}
}

void RZUF3_TextInput::removeTextRenderer()
{
	if(m_textRenderer == nullptr) return;

	RZUF3_Object* textRendererObject = m_textRenderer->getObject();

	_REMOVE_LISTENER(textRendererObject->getEventsManager(), TextSelectionChange);

	g_scene->removeObject(textRendererObject->getName());

	delete m_textRenderer;
	m_textRenderer = nullptr;
}

void RZUF3_TextInput::createTextRenderer()
{
	if(m_textRenderer != nullptr) return;

	RZUF3_TextSelRendererOptions textRendererOptions;
	textRendererOptions.style = mp_options.style.textStyle;
	textRendererOptions.text = m_text;
	textRendererOptions.useLangFile = false;
	textRendererOptions.alignment = RZUF3_Align_TopLeft;
	textRendererOptions.useOnDraw = false;
	textRendererOptions.useOnHoverCursor = false;
	textRendererOptions.preventCtrlX = true;

	m_textRenderer = new RZUF3_TextSelRenderer(textRendererOptions);

	RZUF3_ObjectDefinition textRendererDef;
	textRendererDef.name = m_object->getName() + "_text";
	textRendererDef.scripts = { m_textRenderer };

	RZUF3_Object* textRendererObject = g_scene->addObject(textRendererDef);

	if(textRendererObject == nullptr) {
		spdlog::error("Failed to create text renderer object for text input");
		delete m_textRenderer;
		m_textRenderer = nullptr;
		return;
	}

	textRendererObject->setParent(m_object);
	textRendererObject->init();

	_ADD_LISTENER(textRendererObject->getEventsManager(), TextSelectionChange);
}

void RZUF3_TextInput::updateTextRenderer()
{
	if (m_textRenderer == nullptr) return;

	m_textRenderer->setText(m_text);
}

void RZUF3_TextInput::controlledSetText(std::string text, bool noNewLineCheck)
{
	if (!m_options.multiline && !noNewLineCheck) {
		size_t pos = text.find("\n");
		if (pos != std::string::npos) {
			text = text.substr(0, pos);
		}
	}

	int maxChars = m_options.maxChars;
	if (maxChars > 0 && text.size() > maxChars) {
		text = text.substr(0, maxChars);
	}

	m_text = text;

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	RZUF3_UIStringValueChangeEvent objEvent(m_text);
	objEventsManager->dispatchEvent(&objEvent);

	updateTextRenderer();
	updateCursorRectAndScroll();
}

void RZUF3_TextInput::updateStyle()
{
	if (m_textRenderer == nullptr) return;
	if (m_clickable == nullptr) return;

	RZUF3_TextInputStyle* style = getStyle(m_isFocused);

	m_textRenderer->setStyle(style->textStyle);

	m_textRenderer->setDstPos(
		style->rect.x + style->horizontalPadding,
		style->rect.y + style->verticalPadding
	);

	m_textRenderer->setMaxSize(
		style->rect.w ? (style->rect.w - style->horizontalPadding * 2) : 0,
		style->rect.h ? (style->rect.h - style->verticalPadding * 2) : 0
	);

	m_clickable->setRect({
		style->rect.x,
		style->rect.y,
		style->rect.w ? style->rect.w : m_textRenderer->getWidth() + style->horizontalPadding * 2,
		style->rect.h ? style->rect.h : m_textRenderer->getHeight() + style->verticalPadding * 2
	});

	m_blinkTimer->setInterval(style->blinkTime);
	m_blinkTimer->setEnabled(m_isFocused);
}

void RZUF3_TextInput::updateCursorRectAndScroll()
{
	if(!m_textRenderer) return;

	int cursorPos = m_textRenderer->getSelectionEnd();

	int x, y;
	m_textRenderer->charIndexToPoint(cursorPos, x, y);
	int lineHeight = m_textRenderer->getFontHeight();

	RZUF3_TextInputStyle* style = getStyle(m_isFocused);

	m_cachedCursorRect = {
		style->horizontalPadding + style->rect.x + x - m_scrollX,
		style->verticalPadding + style->rect.y + y - lineHeight - m_scrollY,
		1,
		lineHeight
	};

	int cursorOutX = m_cachedCursorRect.x + style->horizontalPadding - style->rect.x - style->rect.w;
	if (cursorOutX > 0 && style->rect.w > 0) {
		setScroll(m_scrollX + cursorOutX + 8, m_scrollY);
	}

	int cursorOutMinusX = m_cachedCursorRect.x - style->horizontalPadding - style->rect.x;
	if (cursorOutMinusX < 0 && style->rect.w > 0) {
		setScroll(m_scrollX + cursorOutMinusX - 8, m_scrollY);
	}

	int cursorOutY = m_cachedCursorRect.y + lineHeight + style->verticalPadding - style->rect.y - style->rect.h;
	if (cursorOutY > 0 && style->rect.h > 0) {

		setScroll(m_scrollX, m_scrollY + cursorOutY + 8);
	}

	int cursorOutMinusY = m_cachedCursorRect.y - style->verticalPadding - style->rect.y;
	if (cursorOutMinusY < 0 && style->rect.h > 0) {

		setScroll(m_scrollX, m_scrollY + cursorOutMinusY - 8);
	}

	// spdlog::debug("Cursor rect: p={} x={}, y={}, w={}, h={}", m_cursorPos, m_cachedCursorRect.x, m_cachedCursorRect.y, m_cachedCursorRect.w, m_cachedCursorRect.h);
}

void RZUF3_TextInput::setScroll(int x, int y)
{
	if(x < 0) x = 0;
	if(y < 0) y = 0;

	m_scrollX = x;
	m_scrollY = y;

	if(!m_textRenderer) return;

	m_textRenderer->setOffset(x, y);

	updateCursorRectAndScroll();
}
