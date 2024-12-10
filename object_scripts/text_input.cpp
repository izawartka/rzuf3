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

	m_blinkTimer = new RZUF3_Timer("blinkTimer", 0);
	m_object->addScript(m_blinkTimer);

	createClickable(); // main clickable must be created first so it can set focus on text renderer before text renderer's clickable sends its events
	createTextRenderer();
	updateClickable(); // then its rect has to be updated in case of dynamic text size
	createBorderBox();

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, MouseDown);
	_ADD_LISTENER(objEventsManager, MouseDownOutside);
	_ADD_LISTENER(objEventsManager, UISetValue);
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
	_REMOVE_LISTENER(objEventsManager, UISetValue);
	_REMOVE_LISTENER(objEventsManager, SetRect);
	_REMOVE_LISTENER(objEventsManager, Timer);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);
	_REMOVE_LISTENER(eventsManager, TextInputFocus);
	_REMOVE_LISTENER(eventsManager, TextInput);
	_REMOVE_LISTENER(eventsManager, KeyDown);

	removeTextRenderer();
	removeClickable();
	removeBorderBox();

	m_object->removeScript(m_blinkTimer);
	delete m_blinkTimer;
	m_blinkTimer = nullptr;

	m_text.clear();
	m_isFocused = false;
	m_isBlinking = false;
	m_scrollX = 0;
	m_scrollY = 0;
	m_cachedCursorRect = { 0, 0, 0, 0 };
	m_isCtrlDown = false;
	m_isShiftDown = false;
}

void RZUF3_TextInput::setMaxChars(int maxChars)
{
	m_options.maxChars = maxChars;

	controlledSetText(m_text);
}

void RZUF3_TextInput::setStyle(RZUF3_TextInputStyle style, bool focused)
{
	if(focused) m_options.focusedStyle = style;
	else m_options.style = style;

	if(focused != m_isFocused) return;

	updateTextRendererStyle();
	updateBorderBox();
	updateClickable();
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

	RZUF3_TextInputStyle* style = getCurrentStylePtr();

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
		newText = newText.erase(start, end - start);
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

SDL_Rect RZUF3_TextInput::getBorderRect(bool focused)
{
	if (m_textRenderer == nullptr) return { 0, 0, 0, 0 };

	RZUF3_TextInputStyle* style = getCurrentStylePtr();

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
	if(event->getButton() != SDL_BUTTON_LEFT) return;
	setFocused(true);

	m_textRenderer->setFocused(true);
}

void RZUF3_TextInput::onMouseDownOutside(RZUF3_MouseDownOutsideEvent* event)
{
	if (event->getButton() != SDL_BUTTON_LEFT) return;
	if (!m_isFocused) return;

	setFocused(false);
}

void RZUF3_TextInput::onUISetValue(RZUF3_UISetValueEvent* event)
{
	if(event->getTypeIndex() != typeid(std::string)) return;

	std::string value = *(std::string*)event->getValue();
	setText(value);
}

/* updates rect for both focused and unfocused style */
void RZUF3_TextInput::onSetRect(RZUF3_SetRectEvent* event)
{
	SDL_Rect rect = event->getRect();

	m_options.style.rect = rect;
	m_options.focusedStyle.rect = rect;

	updateTextRendererStyle();
	updateBorderBox();
	updateClickable();
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
	RZUF3_TextInputStyle* style = getCurrentStylePtr();

	if (m_borderBox) m_borderBox->draw();
	if (m_textRenderer) m_textRenderer->draw();

	if (m_isBlinking && m_isFocused) {
		g_renderer->setColor(style->blinkColor);
		g_renderer->setAlign(RZUF3_Align_TopLeft);
		g_renderer->fillRect(obj, m_cachedCursorRect);
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

	updateTextRendererStyle();
	updateClickable();
	updateBorderBox();
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
	std::type_index typeIndex = typeid(std::string);
	RZUF3_UIValueChangeEvent objEvent(typeIndex, &m_text, sizeof(std::string));
	objEventsManager->dispatchEvent(&objEvent);

	if(m_textRenderer) m_textRenderer->setText(m_text, false);

	updateClickable();
	updateBorderBox();
	updateCursorRectAndScroll();
}

void RZUF3_TextInput::updateTextRendererStyle()
{
	if (m_textRenderer == nullptr) return;

	RZUF3_TextInputStyle* style = getCurrentStylePtr();

	m_textRenderer->setStyle(style->textStyle);

	m_textRenderer->setDstPos(
		style->rect.x + style->horizontalPadding,
		style->rect.y + style->verticalPadding
	);

	m_textRenderer->setMaxSize(
		style->rect.w ? (style->rect.w - style->horizontalPadding * 2) : 0,
		style->rect.h ? (style->rect.h - style->verticalPadding * 2) : 0
	);

	m_blinkTimer->setInterval(style->blinkTime);
	m_blinkTimer->setEnabled(m_isFocused);
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
	RZUF3_TextInputStyle* style = getCurrentStylePtr();

	RZUF3_TextSelRendererOptions textRendererOptions;
	textRendererOptions.style = m_options.style.textStyle;
	textRendererOptions.text = m_text;
	textRendererOptions.useLangFile = false;
	textRendererOptions.alignment = RZUF3_Align_TopLeft;
	textRendererOptions.useOnDraw = false;
	textRendererOptions.useOnHoverCursor = false;
	textRendererOptions.preventCtrlX = true;
	textRendererOptions.dstRect.x = style->rect.x + style->horizontalPadding;
	textRendererOptions.dstRect.y = style->rect.y + style->verticalPadding;
	textRendererOptions.cropRect.w = style->rect.w ? (style->rect.w - style->horizontalPadding * 2) : 0;
	textRendererOptions.cropRect.h = style->rect.h ? (style->rect.h - style->verticalPadding * 2) : 0;

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

void RZUF3_TextInput::removeBorderBox()
{
	if(m_borderBox == nullptr) return;

	m_object->removeScript(m_borderBox);
	delete m_borderBox;
	m_borderBox = nullptr;
}

void RZUF3_TextInput::createBorderBox()
{
	if(m_borderBox != nullptr) return;

	RZUF3_BorderBoxOptions borderBoxOptions;
	borderBoxOptions.style = getCurrentStylePtr()->borderBoxStyle;
	borderBoxOptions.useOnDraw = false;
	borderBoxOptions.useOnSetRect = false;
	borderBoxOptions.rect = getBorderRect(m_isFocused);

	m_borderBox = new RZUF3_BorderBox(borderBoxOptions);
	m_object->addScript(m_borderBox);
}

void RZUF3_TextInput::updateBorderBox()
{
	if(m_borderBox == nullptr) return;

	m_borderBox->setStyle(getCurrentStylePtr()->borderBoxStyle);
	m_borderBox->setRect(getBorderRect(m_isFocused));
}

void RZUF3_TextInput::removeClickable()
{
	if(m_clickable == nullptr) return;

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;
}

void RZUF3_TextInput::createClickable()
{
	if(m_clickable != nullptr) return;

	RZUF3_ClickableOptions clickableOptions;
	clickableOptions.onHoverCursorId = SDL_SYSTEM_CURSOR_IBEAM;
	clickableOptions.rect = getBorderRect(m_isFocused);

	m_clickable = new RZUF3_Clickable(clickableOptions);
	m_object->addScript(m_clickable);
}

void RZUF3_TextInput::updateClickable()
{
	if(m_clickable == nullptr) return;

	m_clickable->setRect(getBorderRect(m_isFocused));
}

RZUF3_TextInputStyle* RZUF3_TextInput::getCurrentStylePtr()
{
	return m_isFocused ? &m_options.focusedStyle : &m_options.style;
}

void RZUF3_TextInput::updateCursorRectAndScroll(bool noAutoscroll)
{
	if(!m_textRenderer) return;

	int cursorPos = m_textRenderer->getSelectionEnd();

	int x, y;
	m_textRenderer->charIndexToPoint(cursorPos, x, y);
	int lineHeight = m_textRenderer->getFontHeight();

	RZUF3_TextInputStyle* style = getCurrentStylePtr();

	m_cachedCursorRect = {
		style->horizontalPadding + style->rect.x + x - m_scrollX,
		style->verticalPadding + style->rect.y + y - lineHeight - m_scrollY,
		1,
		lineHeight
	};

	if(noAutoscroll) return;

	int cursorOutX = m_cachedCursorRect.x + style->horizontalPadding - style->rect.x - style->rect.w;
	if (cursorOutX > 0 && style->rect.w > 0) {
		setScroll(m_scrollX + cursorOutX + 1, m_scrollY);
	}

	int cursorOutMinusX = m_cachedCursorRect.x - style->horizontalPadding - style->rect.x;
	if (cursorOutMinusX < 0 && style->rect.w > 0) {
		setScroll(m_scrollX + cursorOutMinusX - 1, m_scrollY);
	}

	int cursorOutY = m_cachedCursorRect.y + lineHeight + style->verticalPadding - style->rect.y - style->rect.h;
	if (cursorOutY > 0 && style->rect.h > 0) {

		setScroll(m_scrollX, m_scrollY + cursorOutY + 1);
	}

	int cursorOutMinusY = m_cachedCursorRect.y - style->verticalPadding - style->rect.y;
	if (cursorOutMinusY < 0 && style->rect.h > 0) {

		setScroll(m_scrollX, m_scrollY + cursorOutMinusY - 1);
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

	updateCursorRectAndScroll(true);
}
