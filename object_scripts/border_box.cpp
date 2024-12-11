#include "border_box.h"
#include "../events/events.h"

RZUF3_BorderBox::RZUF3_BorderBox(RZUF3_BorderBoxOptions options)
{
	mp_options = options;
}

RZUF3_BorderBox::~RZUF3_BorderBox()
{
}

void RZUF3_BorderBox::init()
{
	m_options = mp_options;

	setUseOnSetRect(m_options.useOnSetRect);
	setUseOnDraw(m_options.useOnDraw);
}

void RZUF3_BorderBox::deinit()
{
	setUseOnSetRect(false);
	setUseOnDraw(false);
}

void RZUF3_BorderBox::setStyle(RZUF3_BorderBoxStyle style)
{
	m_options.style = style;
}

void RZUF3_BorderBox::setUseOnSetRect(bool useOnSetRect)
{
	m_options.useOnSetRect = useOnSetRect;
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();

	if (useOnSetRect && !m_hasOnSetRectListener)
	{
		_ADD_LISTENER(eventsManager, SetRect);
		m_hasOnSetRectListener = true;
	}
	else if (!useOnSetRect && m_hasOnSetRectListener)
	{
		_REMOVE_LISTENER(eventsManager, SetRect);
		m_hasOnSetRectListener = false;
	}
}

void RZUF3_BorderBox::setUseOnDraw(bool useOnDraw)
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

void RZUF3_BorderBox::setAlignment(RZUF3_Align alignment)
{
	m_options.alignment = alignment;
}

void RZUF3_BorderBox::setRect(SDL_Rect rect)
{
	m_options.rect = rect;
}

void RZUF3_BorderBox::draw()
{
	g_renderer->setAlign(m_options.alignment);
	g_renderer->setColor(m_options.style.color);
	g_renderer->drawRect(m_object, m_options.rect, m_options.style.width);

	if (m_options.style.infillColor.a > 0)
	{
		SDL_Rect infillRect = m_options.rect;
		infillRect.x += m_options.style.width;
		infillRect.y += m_options.style.width;
		infillRect.w -= m_options.style.width * 2;
		infillRect.h -= m_options.style.width * 2;

		g_renderer->setColor(m_options.style.infillColor);
		g_renderer->fillRect(m_object, infillRect);
	}
}

RZUF3_BorderBoxStyle RZUF3_BorderBox::getStyle() const
{
	return m_options.style;
}

bool RZUF3_BorderBox::getUseOnSetRect() const
{
	return m_options.useOnSetRect;
}

bool RZUF3_BorderBox::getUseOnDraw() const
{
	return m_options.useOnDraw;
}

RZUF3_Align RZUF3_BorderBox::getAlignment() const
{
	return m_options.alignment;
}

SDL_Rect RZUF3_BorderBox::getRect() const
{
	return m_options.rect;
}

void RZUF3_BorderBox::onDraw(RZUF3_DrawEvent* event)
{
	if(m_options.useOnDraw) draw();
}

void RZUF3_BorderBox::onSetRect(RZUF3_SetRectEvent* event)
{
	if(m_options.useOnSetRect) setRect(event->getRect());
}
