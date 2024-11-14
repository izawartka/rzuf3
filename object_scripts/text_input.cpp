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
	m_isMultiline = mp_options.multiline;
	m_style = mp_options.style;
	m_focusedStyle = mp_options.focusedStyle;
	setText(mp_options.text);

	m_renderer = g_scene->getRenderer();

	RZUF3_Object* obj = m_object;

	RZUF3_TextRendererOptions textRendererOptions;
	textRendererOptions.fontFilepath = mp_options.fontFilepath;
	textRendererOptions.text = m_text;
	textRendererOptions.style.useLangFile = false;

	m_textRenderer = new RZUF3_TextRenderer(textRendererOptions);
	obj->addScript(m_textRenderer);

	m_clickable = new RZUF3_Clickable();
	obj->addScript(m_clickable);

	m_blinkTimer = new RZUF3_Timer("blinkTimer", m_focusedStyle.blinkTime);
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

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;

	m_object->removeScript(m_blinkTimer);
	delete m_blinkTimer;
	m_blinkTimer = nullptr;

	m_renderer = nullptr;
}

void RZUF3_TextInput::setStyle(RZUF3_TextInputStyle style)
{
	m_style = style;

	updateStyle();
}

void RZUF3_TextInput::setFocusedStyle(RZUF3_TextInputStyle style)
{
	m_focusedStyle = style;

	updateStyle();
}

void RZUF3_TextInput::setText(std::string text)
{
	m_cursorPos = text.size();
	controlledSetText(text);
}

void RZUF3_TextInput::setMultiline(bool multiline)
{
	m_isMultiline = multiline;

	controlledSetText(m_text);
}

void RZUF3_TextInput::setFocuesed(bool focused)
{
	if(focused == m_isFocused) return;

	std::string id = m_object->getName();
	SDL_Rect rect = m_focusedStyle.rect;
	g_renderer->objectToScreenRect(m_object, rect);

	if (focused) {
		g_game->requestTextInput(id, rect);
	}
	else {
		g_game->stopTextInput(id);
	}
}

RZUF3_TextInputStyle RZUF3_TextInput::getStyle() const
{
	return m_style;
}

RZUF3_TextInputStyle RZUF3_TextInput::getFocusedStyle() const
{
	return m_focusedStyle;
}

std::string RZUF3_TextInput::getText() const
{
	return m_text;
}

bool RZUF3_TextInput::getMultiline() const
{
	return m_isMultiline;
}

bool RZUF3_TextInput::getFocused() const
{
	return m_isFocused;
}

void RZUF3_TextInput::onMouseDown(RZUF3_MouseDownEvent* event)
{
	if (m_isFocused) return;

	setFocuesed(true);
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

	m_style.rect = rect;
	m_focusedStyle.rect = rect;

	updateStyle();
}

void RZUF3_TextInput::onTimer(RZUF3_TimerEvent* event)
{
	if(event->getId() != "blinkTimer") return;

	m_isBlinking = !m_isBlinking;
	updateText();
}

void RZUF3_TextInput::onDraw(RZUF3_DrawEvent* event)
{
	RZUF3_Object* obj = m_object;
	RZUF3_TextInputStyle* style = m_isFocused ? &mp_options.focusedStyle : &m_style;

	m_renderer->setColor(style->borderColor);
	m_renderer->setAlign(RZUF3_Align_TopLeft);
	m_renderer->drawRect(obj, {
		style->rect.x,
		style->rect.y,
		style->rect.w,
		style->rect.h ? style->rect.h : m_textRenderer->getHeight() + style->verticalPadding * 2
	}, m_style.borderWidth);
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

	int pos = m_cursorPos;
	if(pos > m_text.size()) pos = m_text.size();
	if(pos < 0) pos = 0;

	std::string addedText = event->getText();
	m_cursorPos += addedText.size();
	controlledSetText(m_text.insert(pos, addedText));
}

void RZUF3_TextInput::onKeyDown(RZUF3_KeyDownEvent* event)
{
	if (!m_isFocused) return;

	std::string text = m_text;
	switch (event->getKeyCode()) {
		case SDLK_BACKSPACE:
			if (text.size() > 0) {
				text.pop_back();
				controlledSetText(text, true);
			}
			break;
		case SDLK_RETURN:
			if (m_isMultiline) {
				text += "\n";
				controlledSetText(text, true);
			}
			break;
		case SDLK_LEFT:
			if (m_cursorPos > 0) {
				m_cursorPos--;
			}
			break;
		case SDLK_RIGHT:
			if (m_cursorPos < m_text.size()) {
				m_cursorPos++;
			}
			break;
		default:
			return;
	}
}

void RZUF3_TextInput::controlledSetText(std::string text, bool noNewLineCheck)
{
	if (!m_isMultiline && !noNewLineCheck) {
		size_t pos = text.find("\n");
		if (pos != std::string::npos) {
			text = text.substr(0, pos);
		}
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
	if (m_clickable == nullptr) return;

	spdlog::debug("Updating style for text input");

	RZUF3_TextInputStyle* style = m_isFocused ? &mp_options.focusedStyle : &m_style;

	RZUF3_TextStyle textStyle = m_textRenderer->getStyle();
	textStyle.color = style->textColor;
	textStyle.bgColor = style->bgColor;
	textStyle.size = style->textSize;
	textStyle.style = style->textStyle;
	textStyle.alignment = RZUF3_Align_TopLeft;
	m_textRenderer->setStyle(textStyle);

	m_textRenderer->setDstPos(
		style->rect.x + style->horizontalPadding,
		style->rect.y + style->verticalPadding
	);

	m_clickable->setRect({
		style->rect.x,
		style->rect.y,
		style->rect.w,
		style->rect.h ? style->rect.h : m_textRenderer->getHeight() + style->verticalPadding * 2
	});

	m_blinkTimer->setInterval(style->blinkTime);
	m_blinkTimer->setEnabled(m_isFocused);

	updateText();
}

void RZUF3_TextInput::updateText()
{
	if (m_textRenderer == nullptr) return;

	std::string text = m_text;
	if (m_isBlinking && m_isFocused) {
		if (m_cursorPos >= text.size()) {
			m_cursorPos = text.size();
			text += "_";
		}
		else if (m_cursorPos >= 0) {
			text[m_cursorPos] = '_';
		}
	}

	m_textRenderer->setText(text);
}
