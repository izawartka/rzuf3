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

	RZUF3_Object* obj = m_object;

	RZUF3_TextRendererOptions textRendererOptions;
	textRendererOptions.fontFilepath = mp_options.fontFilepath;
	textRendererOptions.text = m_text;
	textRendererOptions.style.useLangFile = false;
	textRendererOptions.useOnDraw = false;

	m_textRenderer = new RZUF3_TextRenderer(textRendererOptions);
	obj->addScript(m_textRenderer);

	textRendererOptions.metricsOnly = true;
	m_helperTextRenderer = new RZUF3_TextRenderer(textRendererOptions);
	obj->addScript(m_helperTextRenderer);

	m_clickable = new RZUF3_Clickable();
	obj->addScript(m_clickable);
	m_clickable->setOnHoverCursor(SDL_SYSTEM_CURSOR_IBEAM);

	m_blinkTimer = new RZUF3_Timer("blinkTimer", 0);
	obj->addScript(m_blinkTimer);

	updateStyle();

	RZUF3_EventsManager* objEventsManager = obj->getEventsManager();
	_ADD_LISTENER(objEventsManager, MouseDown);
	_ADD_LISTENER(objEventsManager, MouseDownOutside);
	_ADD_LISTENER(objEventsManager, UISetStringValue);
	_ADD_LISTENER(objEventsManager, SetRect);
	_ADD_LISTENER(objEventsManager, Timer);

	RZUF3_EventsManager* eventsManager = obj->getScene()->getEventsManager();
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

	m_object->removeScript(m_textRenderer);
	delete m_textRenderer;
	m_textRenderer = nullptr;

	m_object->removeScript(m_helperTextRenderer);
	delete m_helperTextRenderer;
	m_helperTextRenderer = nullptr;

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
}

void RZUF3_TextInput::setFocusedStyle(RZUF3_TextInputStyle style)
{
	m_options.focusedStyle = style;

	updateStyle();
}

void RZUF3_TextInput::setText(std::string text)
{
	m_cursorPos = text.size();

	controlledSetText(text);
	controlledSetCursorPos(m_cursorPos, true);
}

void RZUF3_TextInput::setMultiline(bool multiline)
{
	m_options.multiline = multiline;

	controlledSetText(m_text);
	controlledSetCursorPos(m_cursorPos, true);
}

void RZUF3_TextInput::setFocuesed(bool focused)
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
	m_cursorPos = pos;
	controlledSetCursorPos(m_cursorPos);
}

void RZUF3_TextInput::setCursorPosFromXY(int x, int y)
{
	if (m_textRenderer == nullptr) return;

	int charIndex = m_textRenderer->pointToCharIndex(
		x + m_scrollX - m_options.style.horizontalPadding,
		y + m_scrollY - m_options.style.verticalPadding
	);
	if(charIndex > -1) controlledSetCursorPos(charIndex);
}

void RZUF3_TextInput::addText(std::string text)
{
	int pos = m_cursorPos;
	if (pos > m_text.size()) pos = m_text.size();
	if (pos < 0) pos = 0;

	controlledSetText(m_text.insert(pos, text));
	controlledSetCursorPos(pos + text.size());
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

	return {
		style->rect.x,
		style->rect.y,
		style->rect.w ? style->rect.w : m_textRenderer->getWidth() + style->horizontalPadding * 2,
		style->rect.h ? style->rect.h : m_textRenderer->getHeight() + style->verticalPadding * 2
	};
}

void RZUF3_TextInput::onMouseDown(RZUF3_MouseDownEvent* event)
{
	if (!m_isFocused) setFocuesed(true);

	int x = event->getX();
	int y = event->getY();

	setCursorPosFromXY(x, y);
}

void RZUF3_TextInput::onMouseDownOutside(RZUF3_MouseDownOutsideEvent* event)
{
	if (!m_isFocused) return;

	setFocuesed(false);
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

	updateStyle();
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
		case SDLK_BACKSPACE:
			if (m_cursorPos > 0) {
				text.erase(m_cursorPos - 1, 1);
				controlledSetText(text, true);
				controlledSetCursorPos(m_cursorPos - 1, true);
			}
			break;
		case SDLK_DELETE:
			if (m_cursorPos < text.size()) {
				text.erase(m_cursorPos, 1);
				controlledSetText(text, true);
				controlledSetCursorPos(m_cursorPos, true);
			}
			break;
		case SDLK_RETURN:
			if (m_options.multiline) addText("\n");
			break;
		case SDLK_LEFT:
			controlledSetCursorPos(m_cursorPos - 1);
			break;
		case SDLK_RIGHT:
			controlledSetCursorPos(m_cursorPos + 1);
			break;
		case SDLK_HOME:
			controlledSetCursorPos(0);
			break;
		case SDLK_END:
			controlledSetCursorPos(text.size());
			break;
		default:
			return;
	}
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

	updateText();
}

void RZUF3_TextInput::updateStyle()
{
	if (m_textRenderer == nullptr) return;
	if (m_helperTextRenderer == nullptr) return;
	if (m_clickable == nullptr) return;

	RZUF3_TextInputStyle* style = getStyle(m_isFocused);

	RZUF3_TextStyle textStyle = m_textRenderer->getStyle();
	textStyle.color = style->textColor;
	textStyle.bgColor = style->bgColor;
	textStyle.size = style->textSize;
	textStyle.style = style->textStyle;
	textStyle.alignment = RZUF3_Align_TopLeft;

	m_textRenderer->setStyle(textStyle);
	m_helperTextRenderer->setStyle(textStyle);

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

	updateText();
	controlledSetCursorPos(m_cursorPos, true);
}

void RZUF3_TextInput::updateText()
{
	if (m_textRenderer == nullptr) return;

	// add space to last line if it's empty
	std::string text = m_text;
	std::string lastLine = m_text;
	size_t pos = m_text.find_last_of("\n");
	if (pos != std::string::npos) {
		lastLine = lastLine.substr(pos + 1);
	}

	if (lastLine.size() == 0) {
		text += " ";
	}

	m_textRenderer->setText(text);
}

void RZUF3_TextInput::controlledSetCursorPos(int pos, bool force)
{
	if(pos == m_cursorPos && !force) return;
	m_cursorPos = pos;

	std::string text = m_text;
	if (m_cursorPos < 0) {
		m_cursorPos = 0;
	} else if (m_cursorPos > text.size()) {
		m_cursorPos = text.size();
	} 

	if (m_helperTextRenderer == nullptr) return;
	std::string cutText = text.substr(0, m_cursorPos);

	int lineCount = std::count(cutText.begin(), cutText.end(), '\n') + 1;

	if (lineCount > 1) {
		size_t pos = cutText.find_last_of("\n");
		if (pos != std::string::npos) {
			cutText = cutText.substr(pos + 1);
		}
	}

	bool lastLineEmpty = cutText.size() == 0;
	m_helperTextRenderer->setText(lastLineEmpty ? " " : cutText);

	int x = m_helperTextRenderer->getWidth();
	int lineHeight = m_helperTextRenderer->getHeight();
	int bottomY = lineHeight * lineCount;

	if (lastLineEmpty) x = 0;

	RZUF3_TextInputStyle* style = getStyle(m_isFocused);

	m_cachedCursorRect = {
		style->horizontalPadding + style->rect.x + x - m_scrollX,
		style->verticalPadding + style->rect.y + bottomY - lineHeight - m_scrollY,
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

	controlledSetCursorPos(m_cursorPos, true);
}
