#pragma once
#include "../events/events.h"
#include "../event_macros.h"
#include "../renderer.h"
#include "object_scripts.h"
#include "drop_down_style.h"

class RZUF3_BorderBox;
class RZUF3_Clickable;
class RZUF3_TextButton;

struct RZUF3_DropDownOptions {
	RZUF3_DropDownStyle style;
	RZUF3_DropDownItems items;
	int selectedItemIndex = 0;
	bool useOnSetRect = true;
	bool sendValueAsString = true;
	RZUF3_Align alignment = RZUF3_Align_TopLeft; /// TODO
};

class RZUF3_DropDown : public RZUF3_ObjectScript {
public:
	RZUF3_DropDown(std::string fontFilepath, RZUF3_DropDownItems items);
	RZUF3_DropDown(RZUF3_DropDownOptions options);
	~RZUF3_DropDown();

	void init();
	void deinit();

	void setStyle(RZUF3_DropDownStyle style);
	void setUseOnSetRect(bool useOnSetRect);
	void setItems(RZUF3_DropDownItems items);
	void setSelectedItemIndex(int selectedItemIndex, bool endSelecting = false);
	void setAlignment(RZUF3_Align alignment);
	void setSelecting(bool isSelecting);

	RZUF3_DropDownStyle getStyle() const { return m_options.style; }
	bool getUseOnSetRect() const { return m_options.useOnSetRect; }
	RZUF3_DropDownItems getItems() const { return m_options.items; }
	int getSelectedItemIndex() const { return m_options.selectedItemIndex; }
	RZUF3_Align getAlignment() const { return m_options.alignment; }
	bool getSelecting() const { return m_isSelecting; }

private:
	void onSetRect(RZUF3_SetRectEvent* event);
	void onUIButtonClick(RZUF3_UIButtonClickEvent* event);
	void onUISetValue(RZUF3_UISetValueEvent* event);
	void onMouseDownOutside(RZUF3_MouseDownOutsideEvent* event);

	void removeButton();
	void createButton();
	void removeClickable();
	void createClickable();
	void removeItemsBorder();
	void createItemsBorder();
	void removeItemButtons();
	void createItemButtons();

	void updateStyle();
	void updatePositions();
	void updateSelectedItem();
	void cacheLangFileTexts();

	RZUF3_DropDownOptions mp_options;

	RZUF3_DropDownOptions m_options;
	bool m_hasOnSetRectListener = false;
	RZUF3_TextButton* m_button = nullptr;
	RZUF3_BorderBox* m_itemsBorder = nullptr;
	RZUF3_Clickable* m_clickable = nullptr;
	std::vector<RZUF3_TextButton*> m_itemButtons;
	std::vector<std::string> m_cachedLangFileTexts;
	bool m_isSelecting = false;
	RZUF3_DropDownItem* m_selectedItem = nullptr;
	int m_maxWidth = 0;
	SDL_Rect m_itemsRect = { 0, 0, 0, 0 };
	SDL_Rect m_expandedRect = { 0, 0, 0, 0 };

	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(SetRect)
	_DECLARE_LISTENER_MULTI(UIButtonClick)
	_DECLARE_LISTENER(UISetValue)
	_DECLARE_LISTENER(MouseDownOutside)
};