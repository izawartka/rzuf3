#include "drop_down.h"
#include "../lang.h"

RZUF3_DropDown::RZUF3_DropDown(std::string fontFilepath, RZUF3_DropDownItems items)
{
	mp_options.style.button.styles[0].textStyle.fontFilepath = fontFilepath;
	mp_options.style.button.copyFromNormal();

	mp_options.style.item.styles[0].textStyle.fontFilepath = fontFilepath;
	mp_options.style.item.copyFromNormal();

	mp_options.style.defaultColors();

	mp_options.items = items;
}

RZUF3_DropDown::RZUF3_DropDown(RZUF3_DropDownOptions options)
{
	mp_options = options;
}

RZUF3_DropDown::~RZUF3_DropDown()
{
}

void RZUF3_DropDown::init()
{
	m_options = mp_options;

	cacheLangFileTexts();
	updateSelectedItem();
	createButton();
	createItemButtons();
	updatePositions();
	createClickable();
	removeItemButtons();

	setUseOnSetRect(m_options.useOnSetRect);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, LangChange);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, UISetValue);
}

void RZUF3_DropDown::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, LangChange);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, UISetValue);

	setUseOnSetRect(false);

	setSelecting(false);
	removeClickable();
	removeButton();

	m_selectedItem = nullptr;
	m_itemsRect = { 0, 0, 0, 0 };
	m_maxWidth = 0;
	m_cachedLangFileTexts.clear();
}

void RZUF3_DropDown::setStyle(RZUF3_DropDownStyle style)
{
	m_options.style = style;
	updateStyle();
}

void RZUF3_DropDown::setUseOnSetRect(bool useOnSetRect)
{
	m_options.useOnSetRect = useOnSetRect;
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();

	if (useOnSetRect && !m_hasOnSetRectListener) {
		_ADD_LISTENER(objEventsManager, SetRect);
		m_hasOnSetRectListener = true;
	}
	else if (!useOnSetRect && m_hasOnSetRectListener) {
		_REMOVE_LISTENER(objEventsManager, SetRect);
		m_hasOnSetRectListener = false;
	}
}

void RZUF3_DropDown::setItems(RZUF3_DropDownItems items)
{
	m_options.items = items;

	cacheLangFileTexts();
	removeItemButtons();
	createItemButtons();
	updateSelectedItem();
	updatePositions();
	if(!m_isSelecting) removeItemButtons();
}

void RZUF3_DropDown::setSelectedItemIndex(int selectedItemIndex, bool endSelecting)
{
	if (selectedItemIndex < 0 || selectedItemIndex >= m_options.items.size()) return;

	m_options.selectedItemIndex = selectedItemIndex;
	createItemButtons();
	updateSelectedItem();
	updatePositions();
	if (endSelecting) setSelecting(false);
	else if (!m_isSelecting) removeItemButtons();

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	std::type_index typeIndex = typeid(std::string);

	if (m_options.sendValueAsString) {
		std::string value = m_selectedItem ? m_selectedItem->value : "";
		RZUF3_UIValueChangeEvent event(typeIndex, &value, value.size());
		objEventsManager->dispatchEvent(&event);
		return;
	}

	typeIndex = typeid(int);
	RZUF3_UIValueChangeEvent event(typeIndex, &selectedItemIndex, sizeof(int));
	objEventsManager->dispatchEvent(&event);
}

void RZUF3_DropDown::setAlignment(RZUF3_Align alignment)
{
	m_options.alignment = alignment;

	createItemButtons();
	updatePositions();
	if(!m_isSelecting) removeItemButtons();
}

void RZUF3_DropDown::setSelecting(bool isSelecting)
{
	if(m_isSelecting == isSelecting) return;

	m_isSelecting = isSelecting;

	if (m_isSelecting) {
		createItemsBorder();
		createItemButtons();
		updatePositions();
	}
	else {
		removeItemsBorder();
		removeItemButtons();
	}
}

void RZUF3_DropDown::onLangChange(RZUF3_LangChangeEvent* event)
{
	cacheLangFileTexts();
	createItemButtons();
	updateSelectedItem();
	updatePositions();
	if(!m_isSelecting) removeItemButtons();
}

void RZUF3_DropDown::onSetRect(RZUF3_SetRectEvent* event)
{
	RZUF3_DropDownStyle style = m_options.style;
	for (size_t i = 0; i < RZUF3_TEXT_BUTTON_STATE_COUNT; i++)
	{
		style.button.styles[i].rect = event->getRect();
	}

	setStyle(style);
}

void RZUF3_DropDown::onUIButtonClick(RZUF3_UIButtonClickEvent* event)
{
	std::string id = event->getID();

	if (id == m_button->getObject()->getName()) {
		setSelecting(!m_isSelecting);
		return;
	}

	std::string objectName = m_object->getName();
	if (!id._Starts_with(objectName + "_item_")) return;

	size_t index = std::stoi(id.substr(objectName.size() + 6));
	setSelectedItemIndex(index, true);
}

void RZUF3_DropDown::onUISetValue(RZUF3_UISetValueEvent* event)
{
	std::type_index typeIndex = event->getTypeIndex();
	if (typeIndex == typeid(int))
	{
		int value = *static_cast<int*>(event->getValue());
		setSelectedItemIndex(value);
	}
	else if (typeIndex != typeid(std::string)) return;

	std::string value = *static_cast<std::string*>(event->getValue());
	auto it = std::find_if(m_options.items.begin(), m_options.items.end(), [&value](const RZUF3_DropDownItem& item) {
		return item.value == value;
	});

	if(it == m_options.items.end()) return;

	int index = std::distance(m_options.items.begin(), it);
	setSelectedItemIndex(index);
}

void RZUF3_DropDown::onMouseDownOutside(RZUF3_MouseDownOutsideEvent* event)
{
	setSelecting(false);
}

void RZUF3_DropDown::removeButton()
{
	if (m_button == nullptr) return;

	RZUF3_Object* buttonObj = m_button->getObject();

	RZUF3_EventsManager* btnEventsManager = buttonObj->getEventsManager();
	_REMOVE_LISTENER_MULTI(btnEventsManager, UIButtonClick, RZUF3, 0);

	g_scene->removeObject(buttonObj->getName());

	delete m_button;
	m_button = nullptr;
}

void RZUF3_DropDown::createButton()
{
	if (m_button != nullptr) return;

	RZUF3_TextButtonOptions options;
	options.alignment = RZUF3_Align_TopLeft;
	options.styleSet = m_options.style.button;
	options.text = m_selectedItem ? m_cachedLangFileTexts[m_options.selectedItemIndex] : "";
	options.useLangFile = false;
	options.useMouseEvents = true;
	options.useOnSetRect = false;

	m_button = new RZUF3_TextButton(options);

	RZUF3_ObjectDefinition objDef;
	objDef.name = m_object->getName() + "_button";
	objDef.scripts = { m_button };
	RZUF3_Object* obj = g_scene->addObject(objDef);
	
	obj->setParent(m_object);
	obj->init();

	RZUF3_EventsManager* btnEventsManager = obj->getEventsManager();
	_ADD_LISTENER_MULTI(btnEventsManager, UIButtonClick, RZUF3, 0);
}

void RZUF3_DropDown::removeClickable()
{
	if (m_clickable == nullptr) return;

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, MouseDownOutside);
}

void RZUF3_DropDown::createClickable()
{
	if (m_clickable != nullptr) return;

	RZUF3_ClickableOptions options;
	options.rect = m_expandedRect;
	options.alignment = RZUF3_Align_TopLeft;
	options.useOnSetRect = false;

	m_clickable = new RZUF3_Clickable(options);

	m_object->addScript(m_clickable);

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, MouseDownOutside);
}

void RZUF3_DropDown::removeItemsBorder()
{
	if (m_itemsBorder == nullptr) return;

	m_object->removeScript(m_itemsBorder);
	delete m_itemsBorder;
	m_itemsBorder = nullptr;
}

void RZUF3_DropDown::createItemsBorder()
{
	if (m_itemsBorder != nullptr) return;

	RZUF3_BorderBoxOptions options;
	options.style = m_options.style.itemsBorder;
	options.useOnSetRect = false;
	options.useOnDraw = true;
	options.alignment = RZUF3_Align_TopLeft;
	options.rect = m_itemsRect;

	m_itemsBorder = new RZUF3_BorderBox(options);

	m_object->addScript(m_itemsBorder);
}

void RZUF3_DropDown::removeItemButtons()
{
	for(size_t i = 0; i < m_itemButtons.size(); i++)
	{
		RZUF3_TextButton* itemButton = m_itemButtons[i];
		RZUF3_Object* obj = itemButton->getObject();

		_REMOVE_LISTENER_MULTI(obj->getEventsManager(), UIButtonClick, RZUF3, 1 + i);

		g_scene->removeObject(obj->getName());

		delete itemButton;
	}

	m_itemButtons.clear();
}

void RZUF3_DropDown::createItemButtons()
{
	if(m_itemButtons.size() > 0) return;

	for (size_t i = 0; i < m_options.items.size(); i++)
	{
		RZUF3_DropDownItem& item = m_options.items[i];

		RZUF3_TextButtonOptions options;
		options.alignment = RZUF3_Align_TopLeft;
		options.styleSet = m_options.style.item;
		options.text = m_cachedLangFileTexts[i];
		options.useLangFile = false;
		options.useMouseEvents = true;
		options.useOnSetRect = false;
		options.useOnDraw = true;

		RZUF3_TextButton* itemButton = new RZUF3_TextButton(options);

		RZUF3_ObjectDefinition objDef;
		objDef.name = m_object->getName() + "_item_" + std::to_string(i);
		objDef.scripts = { itemButton };

		RZUF3_Object* obj = g_scene->addObject(objDef);
		obj->setParent(m_object);
		obj->init();

		m_itemButtons.push_back(itemButton);

		_ADD_LISTENER_MULTI(obj->getEventsManager(), UIButtonClick, RZUF3, 1 + i);
	}
}

void RZUF3_DropDown::updateStyle()
{
	createItemButtons();

	if(m_button != nullptr) m_button->setStyleSet(m_options.style.button);
	if(m_itemsBorder != nullptr) m_itemsBorder->setStyle(m_options.style.itemsBorder);

	for (auto& itemButton : m_itemButtons)
	{
		itemButton->setStyleSet(m_options.style.item);
	}

	updatePositions();
	if(!m_isSelecting) removeItemButtons();
}

void RZUF3_DropDown::updatePositions()
{
	/// TODO: aligns support
	/// TODO: wrap length support(?)

	if (!m_button) return;

	SDL_Rect buttonRect = m_button->getBorderRect();
	int maxWidth = buttonRect.w;
	int currentItemsHeight = 0;

	for (auto& itemButton : m_itemButtons)
	{
		SDL_Rect itemRect = itemButton->getBorderRect();
		currentItemsHeight += itemRect.h;
		int itemWidth = itemRect.w;
		if (itemWidth > maxWidth) maxWidth = itemWidth;
	}

	m_itemsRect = {
		buttonRect.x,
		buttonRect.y + buttonRect.h,
		maxWidth,
		currentItemsHeight
	};

	m_expandedRect = {
		buttonRect.x,
		buttonRect.y,
		maxWidth,
		buttonRect.h + currentItemsHeight
	};

	SDL_Rect newButtonRect = buttonRect;
	newButtonRect.w = maxWidth;

	if(m_clickable) m_clickable->setRect(m_expandedRect);
	if(m_itemsBorder) m_itemsBorder->setRect(m_itemsRect);
	m_button->setRect(newButtonRect);

	int y = m_itemsRect.y;
	for (auto& itemButton : m_itemButtons)
	{
		SDL_Rect itemRect = itemButton->getBorderRect();
		itemRect.x = m_itemsRect.x;
		itemRect.y = y;
		itemRect.w = m_itemsRect.w;

		itemButton->setRect(itemRect);

		y += itemRect.h;
	}
}

void RZUF3_DropDown::updateSelectedItem()
{
	if (m_options.selectedItemIndex < 0 || m_options.selectedItemIndex >= m_options.items.size())
	{
		m_selectedItem = nullptr;
		if(m_button != nullptr) m_button->setText("");
		return;
	}

	m_selectedItem = &m_options.items[m_options.selectedItemIndex];
	if(m_button != nullptr) m_button->setText(m_cachedLangFileTexts[m_options.selectedItemIndex]);
}

void RZUF3_DropDown::cacheLangFileTexts()
{
	m_cachedLangFileTexts.clear();

	for (auto& item : m_options.items)
	{
		std::string text = item.useLangFile ? g_lang->getText(item.text) : item.text;
		m_cachedLangFileTexts.push_back(text);
	}
}
