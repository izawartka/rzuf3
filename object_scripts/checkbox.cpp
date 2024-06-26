#include "checkbox.h"

RZUF3_Checkbox::RZUF3_Checkbox(std::string fontFilepath, std::string text)
{
	mp_options.fontFilepath = fontFilepath;
	mp_options.text = text;
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
	m_style = mp_options.style;
	m_renderer = g_scene->getRenderer();
	m_checked = mp_options.checked;

	RZUF3_Object* obj = m_object;

	m_textRenderer = new RZUF3_TextRenderer(
		mp_options.fontFilepath, 
		mp_options.text
	);
	obj->addScript(m_textRenderer);

	m_clickable = new RZUF3_Clickable();
	obj->addScript(m_clickable);

	updateStyle();

	RZUF3_EventsManager* objEventsManager = obj->getEventsManager();
	_ADD_LISTENER(objEventsManager, MouseDown);
	_ADD_LISTENER(objEventsManager, UISetValue);

	RZUF3_EventsManager* eventsManager = obj->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
}

void RZUF3_Checkbox::deinit()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, MouseDown);
	_REMOVE_LISTENER(objEventsManager, UISetValue);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);

	m_object->removeScript(m_textRenderer);
	delete m_textRenderer;
	m_textRenderer = nullptr;

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;

	m_renderer = nullptr;
}

void RZUF3_Checkbox::setStyle(RZUF3_CheckboxStyle style)
{
	m_style = style;
	updateStyle();
}

void RZUF3_Checkbox::setChecked(bool checked)
{
	m_checked = checked;

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	RZUF3_UIValueChangeEvent objEvent(m_checked ? 1 : 0);
	objEventsManager->dispatchEvent(&objEvent);
}

void RZUF3_Checkbox::setText(std::string text)
{
	if (!m_textRenderer) return;

	m_textRenderer->setText(text);

	updateStyle();
}

RZUF3_CheckboxStyle RZUF3_Checkbox::getStyle() const
{
	return m_style;
}

bool RZUF3_Checkbox::getChecked() const
{
	return m_checked;
}

void RZUF3_Checkbox::onMouseDown(RZUF3_MouseDownEvent* event)
{
	setChecked(!m_checked);
}

void RZUF3_Checkbox::onUISetValue(RZUF3_UISetValueEvent* event)
{
	setChecked(event->getValue() != 0);
}

void RZUF3_Checkbox::onDraw(RZUF3_DrawEvent* event)
{
	RZUF3_Object* obj = m_object;

	m_renderer->setColor(m_style.borderColor);
	m_renderer->setAlign(RZUF3_Align_TopLeft);
	m_renderer->drawRect(obj, {
		0,
		0,
		m_style.rectSize,
		m_style.rectSize
	}, m_style.borderWidth);

	if(!m_checked) return;

	int totOffset = m_style.checkOffset + m_style.borderWidth;

	m_renderer->setColor(m_style.checkColor);
	m_renderer->setAlign(RZUF3_Align_TopLeft);
	m_renderer->fillRect(obj, {
		totOffset,
		totOffset,
		m_style.rectSize - totOffset * 2,
		m_style.rectSize - totOffset * 2
	});
}

void RZUF3_Checkbox::updateStyle()
{
	if (m_textRenderer == nullptr) return;
	if (m_clickable == nullptr) return;

	RZUF3_TextStyle textStyle = m_textRenderer->getStyle();
	textStyle.color = m_style.textColor;
	textStyle.bgColor = m_style.bgColor;
	textStyle.size = m_style.textSize;
	textStyle.style = m_style.textStyle;
	m_textRenderer->setStyle(textStyle);

	int textHeight = m_textRenderer->getHeight();
	int yOffset = m_style.rectSize - textHeight;

	m_textRenderer->setDstPos(
		m_style.rectSize + m_style.textOffset,
		yOffset / 2
	);

	m_clickable->setRect({
		0,
		std::min(yOffset / 2, 0),
		m_style.rectSize + m_style.textOffset + m_textRenderer->getWidth(),
		std::max(m_style.rectSize, textHeight)
	});

}