#include "checkbox.h"

RZUF3_Checkbox::RZUF3_Checkbox(std::string fontFilepath, std::string text)
{
	mp_options.style.textStyle.fontFilepath = fontFilepath;
	mp_options.labelText = text;
}

RZUF3_Checkbox::RZUF3_Checkbox(RZUF3_CheckboxOptions options)
{
	mp_options = options;
}

RZUF3_Checkbox::~RZUF3_Checkbox()
{
}

void RZUF3_Checkbox::init()
{
	m_options = mp_options;

	createTextRenderer();
	createBorderBoxes();

	setUseOnDraw(m_options.useOnDraw);
	setUseMouseEvents(m_options.useMouseEvents);
	setUseLangFile(m_options.useLangFile);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, UISetValue);
}

void RZUF3_Checkbox::deinit()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, UISetValue);

	setUseOnDraw(false);
	setUseMouseEvents(false);
	setUseLangFile(false);

	removeTextRenderer();
	removeBorderBoxes();
	removeCombinedTexture();

	m_subScriptsOptionsChanged = false;
}

void RZUF3_Checkbox::setStyle(RZUF3_CheckboxStyle style)
{
	m_options.style = style;

	removeCombinedTexture();
	m_subScriptsOptionsChanged = true;
}

void RZUF3_Checkbox::setLabelText(std::string text)
{
	m_options.labelText = text;

	removeCombinedTexture();
	m_subScriptsOptionsChanged = true;
}

void RZUF3_Checkbox::setChecked(bool checked)
{
	m_options.checked = checked;

	removeCombinedTexture();
	sendValueChangeEvent();
}

void RZUF3_Checkbox::setUseOnDraw(bool useOnDraw)
{
	m_options.useOnDraw = useOnDraw;
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();

	if (useOnDraw && !m_hasOnDrawListener)
	{
		_ADD_LISTENER(eventsManager, Draw);
		m_hasOnDrawListener = true;
	}
	else if (!useOnDraw && m_hasOnDrawListener)
	{
		_REMOVE_LISTENER(eventsManager, Draw);
		m_hasOnDrawListener = false;
	}
}

void RZUF3_Checkbox::setUseMouseEvents(bool useMouseEvents)
{
	m_options.useMouseEvents = useMouseEvents;
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();

	if (useMouseEvents && !m_hasMouseEventsListener)
	{
		_ADD_LISTENER(objEventsManager, MouseDown);
		_ADD_LISTENER(objEventsManager, MouseUp);
		_ADD_LISTENER(objEventsManager, MouseUpOutside);

		m_hasMouseEventsListener = true;

		createClickable();
	}
	else if (!useMouseEvents && m_hasMouseEventsListener)
	{
		_REMOVE_LISTENER(objEventsManager, MouseDown);
		_REMOVE_LISTENER(objEventsManager, MouseUp);
		_REMOVE_LISTENER(objEventsManager, MouseUpOutside);

		m_pressed = false;
		m_hasMouseEventsListener = false;

		removeClickable();
	}
}

void RZUF3_Checkbox::setUseLangFile(bool useLangFile)
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

	if (wasTheSame) return;
	removeCombinedTexture();
	m_subScriptsOptionsChanged = true;
}

void RZUF3_Checkbox::setAlignment(RZUF3_Align alignment)
{
	m_options.alignment = alignment;

	removeCombinedTexture();
}

void RZUF3_Checkbox::draw()
{
	if(m_combinedTexture == nullptr) createCombinedTexture();

	SDL_Rect dstRect = getRect();

	g_renderer->setAlign(m_options.alignment);
	g_renderer->drawTexture(m_object, m_combinedTexture, nullptr, dstRect);
}

SDL_Rect RZUF3_Checkbox::getRect() const
{
	if(m_textRenderer == nullptr) return {0, 0, 0, 0};

	int width = m_options.style.rectSize + m_options.style.textOffset + m_textRenderer->getWidth();
	int height = std::max(m_textRenderer->getHeight(), m_options.style.rectSize);

	return {0, 0, width, height};
}

void RZUF3_Checkbox::onDraw(RZUF3_DrawEvent* event)
{
	if (!m_options.useOnDraw) return;
	draw();
}

void RZUF3_Checkbox::onLangChange(RZUF3_LangChangeEvent* event)
{
	if(!m_options.useLangFile) return;

	removeCombinedTexture();
	m_subScriptsOptionsChanged = true;
}

void RZUF3_Checkbox::onMouseDown(RZUF3_MouseDownEvent* event)
{
	if(!m_options.useMouseEvents) return;

	m_pressed = true;
}

void RZUF3_Checkbox::onMouseUp(RZUF3_MouseUpEvent* event)
{
	if(!m_options.useMouseEvents) return;
	if (!m_pressed) return;
	m_pressed = false;

	setChecked(!m_options.checked);
}

void RZUF3_Checkbox::onMouseUpOutside(RZUF3_MouseUpOutsideEvent* event)
{
	if(!m_options.useMouseEvents) return;
	m_pressed = false;
}

void RZUF3_Checkbox::onUISetValue(RZUF3_UISetValueEvent* event)
{
	if(event->getTypeIndex() != typeid(bool)) return;
	if(event->getValue() == nullptr) return;

	bool checked = *(bool*)event->getValue();
	setChecked(checked);
}

int RZUF3_Checkbox::getHalfRectTextLineHeightDiff() const
{
	int rectSize = m_options.style.rectSize;
	if (m_textRenderer == nullptr) return rectSize;

	int textLineHeight = m_textRenderer->getFontHeight();

	return (rectSize - textLineHeight) / 2;
}

void RZUF3_Checkbox::sendValueChangeEvent()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	std::type_index typeIndex = typeid(bool);
	RZUF3_UIValueChangeEvent objEvent(typeIndex, &m_options.checked, sizeof(bool));
	objEventsManager->dispatchEvent(&objEvent);
}

void RZUF3_Checkbox::removeClickable()
{
	if(m_clickable == nullptr) return;

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;
}

void RZUF3_Checkbox::createClickable()
{
	if(m_clickable != nullptr) return;

	RZUF3_ClickableOptions clickableOptions;
	clickableOptions.alignment = m_options.alignment;
	clickableOptions.rect = getRect();

	m_clickable = new RZUF3_Clickable(clickableOptions);
	m_object->addScript(m_clickable);
}

void RZUF3_Checkbox::updateClickable()
{
	if(m_clickable == nullptr) return;

	m_clickable->setRect(getRect());
	m_clickable->setAlignment(m_options.alignment);
}

void RZUF3_Checkbox::removeTextRenderer()
{
	if(m_textRenderer == nullptr) return;

	m_object->removeScript(m_textRenderer);
	delete m_textRenderer;
	m_textRenderer = nullptr;
}

void RZUF3_Checkbox::createTextRenderer()
{
	if(m_textRenderer != nullptr) return;

	RZUF3_TextRendererOptions textOptions;
	textOptions.text = m_options.labelText;
	textOptions.style = m_options.style.textStyle;
	textOptions.alignment = RZUF3_Align_TopLeft;
	textOptions.useOnDraw = false;
	textOptions.useLangFile = m_options.useLangFile;

	m_textRenderer = new RZUF3_TextRenderer(textOptions);
	m_object->addScript(m_textRenderer);
}

void RZUF3_Checkbox::updateTextRenderer()
{
	if(m_textRenderer == nullptr) return;

	m_textRenderer->setText(m_options.labelText);
	m_textRenderer->setStyle(m_options.style.textStyle);
	m_textRenderer->setUseLangFile(m_options.useLangFile);
}

void RZUF3_Checkbox::removeBorderBoxes()
{
	if (m_rectBorderBox != nullptr)
	{
		m_object->removeScript(m_rectBorderBox);
		delete m_rectBorderBox;
		m_rectBorderBox = nullptr;
	}

	if (m_checkBorderBox != nullptr)
	{
		m_object->removeScript(m_checkBorderBox);
		delete m_checkBorderBox;
		m_checkBorderBox = nullptr;
	}
}

void RZUF3_Checkbox::createBorderBoxes()
{
	if(m_rectBorderBox != nullptr || m_checkBorderBox != nullptr) return;

	int diff = getHalfRectTextLineHeightDiff();
	int baseY = diff < 0 ? -diff : 0;
	int rectSize = m_options.style.rectSize;
	int checkSize = m_options.style.checkSize;
	int checkPadding = (rectSize - checkSize) / 2;

	RZUF3_BorderBoxOptions rectOptions;
	rectOptions.style = m_options.style.rectStyle;
	rectOptions.alignment = RZUF3_Align_TopLeft;
	rectOptions.rect = {0, baseY, rectSize, rectSize};
	rectOptions.useOnDraw = false;

	m_rectBorderBox = new RZUF3_BorderBox(rectOptions);
	m_object->addScript(m_rectBorderBox);

	RZUF3_BorderBoxOptions checkOptions;
	checkOptions.style = m_options.style.checkStyle;
	checkOptions.alignment = RZUF3_Align_TopLeft;
	checkOptions.rect = {checkPadding, baseY + checkPadding, checkSize, checkSize};
	checkOptions.useOnDraw = false;

	m_checkBorderBox = new RZUF3_BorderBox(checkOptions);
	m_object->addScript(m_checkBorderBox);
}

void RZUF3_Checkbox::updateBorderBoxes()
{
	if(m_rectBorderBox == nullptr || m_checkBorderBox == nullptr) return;

	m_rectBorderBox->setStyle(m_options.style.rectStyle);
	m_checkBorderBox->setStyle(m_options.style.checkStyle);

	int diff = getHalfRectTextLineHeightDiff();
	int baseY = diff < 0 ? -diff : 0;
	int rectSize = m_options.style.rectSize;
	int checkSize = m_options.style.checkSize;
	int checkPadding = (rectSize - checkSize) / 2;

	m_rectBorderBox->setRect({0, baseY, rectSize, rectSize});
	m_checkBorderBox->setRect({checkPadding, baseY + checkPadding, checkSize, checkSize});
}

void RZUF3_Checkbox::removeCombinedTexture()
{
	if(m_combinedTexture == nullptr) return;

	SDL_DestroyTexture(m_combinedTexture);
	m_combinedTexture = nullptr;
}

void RZUF3_Checkbox::createCombinedTexture()
{
	if(m_combinedTexture != nullptr) return;

	if (m_subScriptsOptionsChanged) {
		updateTextRenderer();
		updateBorderBoxes();
		updateClickable();
	}

	SDL_Rect rect = getRect();
	SDL_Renderer* renderer = g_renderer->getSDLRenderer();

	SDL_Texture* tempTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		rect.w, rect.h
	);

	SDL_Texture* prevTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, tempTexture);
	bool prevUseObjectPos = g_renderer->getUseObjectPos();
	g_renderer->setUseObjectPos(false);

	int textW = m_textRenderer->getWidth();
	int textH = m_textRenderer->getHeight();
	int textX = m_options.style.rectSize + m_options.style.textOffset;
	int textY = (rect.h - textH) / 2;
	if(textY < 0) textY = 0;

	SDL_Texture* textTexture = m_textRenderer->getTexture();
	g_renderer->drawTexture(nullptr, textTexture, nullptr, {textX, textY, textW, textH});

	m_rectBorderBox->draw();
	if(m_options.checked) m_checkBorderBox->draw();

	if (!g_renderer->createStaticTexture(m_combinedTexture, rect.w, rect.h)) {
		spdlog::error("Checkbox: Failed to create combined texture");
	}

	g_renderer->setUseObjectPos(prevUseObjectPos);
	SDL_SetRenderTarget(renderer, prevTarget);

	SDL_DestroyTexture(tempTexture);
}