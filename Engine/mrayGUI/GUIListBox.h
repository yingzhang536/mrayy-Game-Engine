

#ifndef ___GUIListBox___
#define ___GUIListBox___

#include "compileconfig.h"
#include "IGUIListBox.h"
#include "SColor.h"

#include "FontAttributes.h"
#include "GUIElementFactoryDef.h"


namespace mray{
	class KeyboardEvent;
	class MouseEvent;

namespace GUI{
	class IListItem;
	class GUIListBoxComponent;

class MRAYGUI_API GUIListBox:public IGUIListBox
{
	DECLARE_RTTI
protected:
	ItemList m_itemList;
	GUIListBoxComponent* m_component;
	FontAttributes m_FontAttributes;

	bool _OnKeyboardEvent(KeyboardEvent* e);
	bool _OnMouseEvent(MouseEvent* e);

	void _notifyLostFocus();
public:
	GUIListBox(IGUIManager* manager);
	virtual~GUIListBox();


	int GetItemsCount()const;
	IListItem* GetItem(int index)const;
	int FindItem(const core::UTFString& txt)const;
	int AddItem(IListItem* item);
	int InsertItem(int index,IListItem* item);
	void DeleteItem(int index);
	const ItemList& GetItems()const;
	void SortItems();
	void ClearItems();

	int GetSelectedItem();
	void SetSelectedItem(int i);

	int GetItemFromPos(const math::vector2d& y);

	void SetSelectionColor(const video::SColor& clr);
	const video::SColor& GetSelectionColor()const;

	virtual void Draw(video::IRenderArea*vp);

	virtual IGUIElement* Duplicate();
	virtual FontAttributes* GetFontAttributes();
};

DECLARE_ELEMENT_FACTORY(GUIListBox);

}
}

#endif









