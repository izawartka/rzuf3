#include "text_button.h"
#include "../lang.h"

RZUF3_TextButton::RZUF3_TextButton(std::string fontFilepath, std::string text)
{
	mp_options.styleSet.styles[(int)RZUF3_TextButtonState::Normal].textStyle.fontFilepath = fontFilepath;
	mp_options.styleSet.copyFromNormal();
	mp_options.styleSet.defaultColors();
	mp_options.text = text;
}

RZUF3_TextButton::RZUF3_TextButton(RZUF3_TextButtonOptions options)
{
	mp_options = options;
}

RZUF3_TextButton::~RZUF3_TextButton() {

}

void RZUF3_TextButton::init()
{
	m_options = mp_options;
	m_state = m_options.highlighted ? RZUF3_TextButtonState::NormalHighlighted : RZUF3_TextButtonState::Normal;

	cacheLangFileText();

	createTextRenderer();
	createClickable();
	createBorderBox();

	setUseLangFile(m_options.useLangFile);
	setUseOnSetRect(m_options.useOnSetRect);
	setUseOnDraw(m_options.useOnDraw);
	setUseMouseEvents(m_options.useMouseEvents);
}

void RZUF3_TextButton::deinit()
{
	m_options.useLangFile = false;
	setUseLangFile(false);
	setUseOnSetRect(false);
	setUseOnDraw(false);
	setUseMouseEvents(false);

	removeTextRenderer();
	removeClickable();
	removeBorderBox();

	removeCombinedTexture();
	m_cachedText = "";
}

void RZUF3_TextButton::setStyle(RZUF3_TextButtonStyle style, RZUF3_TextButtonState state)
{
	m_options.styleSet.styles[(int)state] = style;
	if(state == m_state) removeCombinedTexture();
}

void RZUF3_TextButton::setStyleSet(RZUF3_TextButtonStyleSet styleSet)
{
	m_options.styleSet = styleSet;
	removeCombinedTexture();
}

void RZUF3_TextButton::setText(std::string text)
{
	m_options.text = text;
	cacheLangFileText();
	removeCombinedTexture();
}

void RZUF3_TextButton::setHighlighted(bool highlighted)
{
	if(m_options.highlighted == highlighted) return;

	m_options.highlighted = highlighted;

	if (m_state == RZUF3_TextButtonState::Normal && m_options.highlighted)
	{
		m_state = RZUF3_TextButtonState::NormalHighlighted;
		removeCombinedTexture();
	}
	else if (m_state == RZUF3_TextButtonState::NormalHighlighted && !m_options.highlighted)
	{
		m_state = RZUF3_TextButtonState::Normal;
		removeCombinedTexture();
	}
}

void RZUF3_TextButton::setUseLangFile(bool useLangFile)
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
	cacheLangFileText();
	removeCombinedTexture();
}

void RZUF3_TextButton::setUseOnSetRect(bool useOnSetRect)
{
	m_options.useOnSetRect = useOnSetRect;
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();

	if (useOnSetRect && !m_hasOnSetRectListener)
	{
		_ADD_LISTENER(objEventsManager, SetRect);
		m_hasOnSetRectListener = true;
	}
	else if (!useOnSetRect && m_hasOnSetRectListener)
	{
		_REMOVE_LISTENER(objEventsManager, SetRect);
		m_hasOnSetRectListener = false;
	}
}

void RZUF3_TextButton::setUseOnDraw(bool useOnDraw)
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

void RZUF3_TextButton::setUseMouseEvents(bool useMouseEvents)
{
	m_options.useMouseEvents = useMouseEvents;
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();

	if (useMouseEvents && !m_hasMouseEventsListener)
	{
		createClickable();

		_ADD_LISTENER(objEventsManager, MouseEnter);
		_ADD_LISTENER(objEventsManager, MouseLeave);
		_ADD_LISTENER(objEventsManager, MouseDown);
		_ADD_LISTENER(objEventsManager, MouseUp);
		m_hasMouseEventsListener = true;
	}
	else if (!useMouseEvents && m_hasMouseEventsListener)
	{
		_REMOVE_LISTENER(objEventsManager, MouseEnter);
		_REMOVE_LISTENER(objEventsManager, MouseLeave);
		_REMOVE_LISTENER(objEventsManager, MouseDown);
		_REMOVE_LISTENER(objEventsManager, MouseUp);
		m_hasMouseEventsListener = false;

		removeClickable();
	}
}

void RZUF3_TextButton::setAlignment(RZUF3_Align alignment)
{
	m_options.alignment = alignment;
}

void RZUF3_TextButton::setRect(SDL_Rect rect)
{
	for (int i = 0; i < RZUF3_TEXT_BUTTON_STATE_COUNT; i++)
	{
		m_options.styleSet.styles[i].rect = rect;
	}

	removeCombinedTexture();
}

void RZUF3_TextButton::draw()
{
	if (m_combinedTexture == nullptr) createCombinedTexture();

	g_renderer->setAlign(m_options.alignment);
	g_renderer->drawTexture(m_object, m_combinedTexture, nullptr, getBorderRect());
}

void RZUF3_TextButton::setState(RZUF3_TextButtonState state)
{
	if(m_state == state) return;

	m_state = state;
	removeCombinedTexture();
}

SDL_Texture* RZUF3_TextButton::getTexture()
{
	if (m_combinedTexture == nullptr) createCombinedTexture();

	return m_combinedTexture;
}

SDL_Rect RZUF3_TextButton::getBorderRect()
{
	if(m_textRenderer == nullptr) return { 0, 0, 0, 0 };

	RZUF3_TextButtonStyle* style = getCurrentStylePtr();

	int textWidth = m_textRenderer->getWidth();
	int textHeight = m_textRenderer->getHeight();
	std::string& text = m_options.text;
	if (text.size() == 0 || text.find_last_of("\n") == text.size() - 1) {
		textHeight += m_textRenderer->getFontHeight();
	}

	return {
		style->rect.x,
		style->rect.y,
		style->rect.w ? style->rect.w : (textWidth + style->horizontalPadding * 2),
		style->rect.h ? style->rect.h : (textHeight + style->verticalPadding * 2)
	};
}

void RZUF3_TextButton::onDraw(RZUF3_DrawEvent* event)
{
	if (m_options.useOnDraw) draw();
}

void RZUF3_TextButton::onLangChange(RZUF3_LangChangeEvent* event)
{
	if(!m_options.useLangFile) return;

	cacheLangFileText();
	removeCombinedTexture();
}

void RZUF3_TextButton::onMouseEnter(RZUF3_MouseEnterEvent* event)
{
	if(m_state < RZUF3_TextButtonState::Hover) setState(RZUF3_TextButtonState::Hover);
}

void RZUF3_TextButton::onMouseLeave(RZUF3_MouseLeaveEvent* event)
{
	RZUF3_TextButtonState targetState = m_options.highlighted ? RZUF3_TextButtonState::NormalHighlighted : RZUF3_TextButtonState::Normal;
	setState(targetState);
}

void RZUF3_TextButton::onMouseDown(RZUF3_MouseDownEvent* event)
{
	if(m_state < RZUF3_TextButtonState::Pressed) setState(RZUF3_TextButtonState::Pressed);
}

void RZUF3_TextButton::onMouseUp(RZUF3_MouseUpEvent* event)
{
	bool wasPressed = m_state == RZUF3_TextButtonState::Pressed;

	RZUF3_TextButtonState targetState = m_options.highlighted ? RZUF3_TextButtonState::NormalHighlighted : RZUF3_TextButtonState::Normal;
	if(m_clickable->isInside()) targetState = RZUF3_TextButtonState::Hover;
	setState(targetState);

	if (wasPressed && m_clickable->isInside()) {
		RZUF3_UIButtonClickEvent buttonEvent(m_object->getName());
		m_object->getEventsManager()->dispatchEvent(&buttonEvent);
	}
}

void RZUF3_TextButton::onSetRect(RZUF3_SetRectEvent* event)
{
	if(m_options.useOnSetRect) setRect(event->getRect());
}

void RZUF3_TextButton::removeTextRenderer()
{
	if (m_textRenderer == nullptr) return;

	m_object->removeScript(m_textRenderer);
	delete m_textRenderer;
	m_textRenderer = nullptr;
}

void RZUF3_TextButton::createTextRenderer()
{
	if (m_textRenderer != nullptr) return;

	RZUF3_TextButtonStyle* style = getCurrentStylePtr();

	RZUF3_TextRendererOptions options;
	options.text = m_cachedText;
	options.style = style->textStyle;
	options.useOnDraw = false;
	options.useLangFile = false;
	options.alignment = RZUF3_Align_TopLeft;

	options.cropRect.w = style->rect.w == 0 ? 0 : style->rect.w - style->horizontalPadding * 2;
	options.cropRect.h = style->rect.h == 0 ? 0 : style->rect.h - style->verticalPadding * 2;
	options.dstRect.x = style->horizontalPadding;
	options.dstRect.y = style->verticalPadding;

	m_textRenderer = new RZUF3_TextRenderer(options);
	m_object->addScript(m_textRenderer);
}

void RZUF3_TextButton::updateTextRenderer()
{
	if (m_textRenderer == nullptr) return;

	RZUF3_TextButtonStyle* style = getCurrentStylePtr();

	m_textRenderer->setText(m_cachedText);
	m_textRenderer->setStyle(style->textStyle);

	int cropX = style->rect.w == 0 ? 0 : style->rect.w - style->horizontalPadding * 2;
	int cropY = style->rect.h == 0 ? 0 : style->rect.h - style->verticalPadding * 2;
	m_textRenderer->setMaxSize(cropX, cropY);

	int dstX = style->horizontalPadding;
	int dstY = style->verticalPadding;
	m_textRenderer->setDstPos(dstX, dstY);
}

void RZUF3_TextButton::removeClickable()
{
	if (m_clickable == nullptr) return;

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;
}

void RZUF3_TextButton::createClickable()
{
	if (m_clickable != nullptr) return;

	RZUF3_ClickableOptions options;
	options.rect = getBorderRect();
	options.alignment = m_options.alignment;

	m_clickable = new RZUF3_Clickable(options);
	m_object->addScript(m_clickable);
}

void RZUF3_TextButton::updateClickable()
{
	if (m_clickable == nullptr) return;

	m_clickable->setRect(getBorderRect());
	m_clickable->setAlignment(m_options.alignment);
}

void RZUF3_TextButton::removeBorderBox()
{
	if (m_borderBox == nullptr) return;

	m_object->removeScript(m_borderBox);
	delete m_borderBox;
	m_borderBox = nullptr;
}

void RZUF3_TextButton::createBorderBox()
{
	if (m_borderBox != nullptr) return;

	RZUF3_BorderBoxOptions options;
	options.style = getCurrentStylePtr()->borderBoxStyle;
	options.useOnSetRect = false;
	options.useOnDraw = false;
	options.alignment = RZUF3_Align_TopLeft;

	SDL_Rect borderBoxRect = getBorderRect();
	borderBoxRect.x = 0;
	borderBoxRect.y = 0;
	options.rect = borderBoxRect;

	m_borderBox = new RZUF3_BorderBox(options);
	m_object->addScript(m_borderBox);
}

void RZUF3_TextButton::updateBorderBox()
{
	if (m_borderBox == nullptr) return;

	SDL_Rect borderBoxRect = getBorderRect();
	borderBoxRect.x = 0;
	borderBoxRect.y = 0;

	m_borderBox->setRect(borderBoxRect);
	m_borderBox->setStyle(getCurrentStylePtr()->borderBoxStyle);
	m_borderBox->setAlignment(RZUF3_Align_TopLeft);
}

void RZUF3_TextButton::removeCombinedTexture()
{
	if(m_combinedTexture == nullptr) return;

	SDL_DestroyTexture(m_combinedTexture);
	m_combinedTexture = nullptr;
}

void RZUF3_TextButton::createCombinedTexture()
{
	if(m_combinedTexture != nullptr) return;
	if(m_textRenderer == nullptr || m_borderBox == nullptr) return;

	updateTextRenderer();
	updateBorderBox();
	updateClickable();

	RZUF3_TextButtonStyle* style = getCurrentStylePtr();
	SDL_Rect borderRect = getBorderRect();
	SDL_Renderer* renderer = g_renderer->getSDLRenderer();

	SDL_Texture* tempTexture = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
		borderRect.w, borderRect.h
	);

	if (tempTexture == nullptr) return;

	SDL_Texture* prevTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, tempTexture);
	bool prevUseObjectPos = g_renderer->getUseObjectPos();
	g_renderer->setUseObjectPos(false);

	m_borderBox->draw();
	m_textRenderer->draw();

	if (!g_renderer->createStaticTexture(m_combinedTexture, borderRect.w, borderRect.h)) {
		spdlog::error("TextButton: Failed to create combined texture");
	}

	SDL_SetRenderTarget(renderer, prevTarget);
	g_renderer->setUseObjectPos(prevUseObjectPos);

	SDL_DestroyTexture(tempTexture);
}

void RZUF3_TextButton::cacheLangFileText()
{
	m_cachedText = m_options.useLangFile ? g_lang->getText(m_options.text) : m_options.text;
}

RZUF3_TextButtonStyle* RZUF3_TextButton::getCurrentStylePtr()
{
	return &m_options.styleSet.styles[(int)m_state];
}
