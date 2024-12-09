#include "slider.h"

RZUF3_Slider::RZUF3_Slider(int min, int max, int value)
{
	mp_options.min = min;
	mp_options.max = max;
	mp_options.value = value;
}

RZUF3_Slider::RZUF3_Slider(RZUF3_SliderOptions options)
{
	mp_options = options;
}

RZUF3_Slider::~RZUF3_Slider()
{

}

void RZUF3_Slider::init()
{
	m_min = mp_options.min;
	m_max = mp_options.max;
	m_style = mp_options.style;

	m_clickable = new RZUF3_Clickable();
	m_object->addScript(m_clickable);

	updateStyle();
	setValue(mp_options.value);

	m_objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(m_objEventsManager, MousePressed);
	_ADD_LISTENER(m_objEventsManager, UISetValue);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
}

void RZUF3_Slider::deinit()
{
	_REMOVE_LISTENER(m_objEventsManager, MousePressed);
	_REMOVE_LISTENER(m_objEventsManager, UISetValue);
	m_objEventsManager = nullptr;

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;
}

void RZUF3_Slider::setStyle(RZUF3_SliderStyle style)
{
	m_style = style;
	updateStyle();
}

void RZUF3_Slider::setMin(int min)
{
	m_min = min;
	if(m_value < m_min) setValue(m_min);
}

void RZUF3_Slider::setMax(int max)
{
	m_max = max;
	if(m_value > m_max) setValue(m_max);
}

void RZUF3_Slider::setValue(int value)
{
	m_value = value;
	if(m_value < m_min) m_value = m_min;
	if(m_value > m_max) m_value = m_max;

	if (m_objEventsManager == nullptr) return;

	RZUF3_UIValueChangeEvent objEvent(m_value);
	m_objEventsManager->dispatchEvent(&objEvent);
}

RZUF3_SliderStyle RZUF3_Slider::getStyle() const
{
	return m_style;
}

int RZUF3_Slider::getValue() const
{
	return m_value;
}

void RZUF3_Slider::onMousePressed(RZUF3_MousePressedEvent* event)
{
	if(event->getButton() != SDL_BUTTON_LEFT) return;

	double fraction = (double)event->getX() / m_style.width;
	double floatValue = m_min + fraction * (m_max - m_min);
	setValue(std::round(floatValue));
}

void RZUF3_Slider::onUISetValue(RZUF3_UISetValueEvent* event)
{
	setValue(event->getValue());
}

void RZUF3_Slider::onDraw(RZUF3_DrawEvent* event)
{
	int thumbOutSize = (m_style.thumbSize - m_style.sliderSize) / 2;
	int sliderOffset = std::max(thumbOutSize, 0);
	int thumbOffset = std::min(thumbOutSize, 0);
	int sliderHalfSize = m_style.sliderSize / 2;
	int thumbHalfSize = m_style.thumbSize / 2;

	SDL_Rect sliderRect = {
		sliderOffset,
		sliderOffset,
		m_style.width,
		m_style.sliderSize
	};

	g_renderer->setColor(m_style.sliderColor);
	g_renderer->setAlign(RZUF3_Align_TopLeft);
	g_renderer->fillRect(m_object, sliderRect);

	double fraction = (double)(m_value - m_min) / (m_max - m_min);
	int thumbPosX = fraction * (m_style.width - m_style.sliderSize);

	SDL_Rect thumbRect = {
		thumbPosX + thumbOffset,
		thumbOffset,
		m_style.thumbSize,
		m_style.thumbSize
	};

	g_renderer->setColor(m_style.thumbColor);
	g_renderer->setAlign(RZUF3_Align_TopLeft);
	g_renderer->fillCircle(m_object, thumbRect);
}

void RZUF3_Slider::updateStyle()
{
	int sliderOffset = std::max(m_style.thumbSize - m_style.sliderSize, 0) / 2;

	SDL_Rect rect = {
		0,
		0,
		m_style.width + sliderOffset*2,
		std::max(m_style.sliderSize, m_style.thumbSize)
	};

	m_clickable->setRect(rect);
}
