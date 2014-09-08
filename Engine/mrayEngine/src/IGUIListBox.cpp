
#include "stdafx.h"
#include "IGUIListBox.h"

namespace mray
{
namespace GUI
{

	IMPLEMENT_RTTI(IGUIListBox,IGUIElement)

const GUID IGUIListBox::ElementType("GUIListBox");

//////////////////////////////////////////////////////////////////////////

	IMPLEMENT_PROPERTY_TYPE_HEADER(ItemHeight, IGUIListBox, float, mT("ItemHeight"), EPBT_Basic, mT("List item height"), 18);
	IMPLEMENT_PROPERTY_TYPE_GENERIC(ItemHeight, IGUIListBox, float, SetItemHeight, GetItemHeight, core::StringConverter::toString, core::StringConverter::toFloat, false)

	IMPLEMENT_PROPERTY_TYPE_HEADER(Background, IGUIListBox, bool, mT("Background"), EPBT_Basic, mT("List item height"), true);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Background, IGUIListBox, bool, SetBackground, GetBackground, core::StringConverter::toString, core::StringConverter::toBool, false)

IGUIListBox::IGUIListBox(IGUIManager* manager):IGUIElement(ElementType,manager),
OnKeyDown(mT("KeyDown")),OnSelectChange(mT("SelectChange"))
{
	AddEvent(&OnKeyDown);
	AddEvent(&OnSelectChange);

	CPropertieDictionary *dic = 0;
	if (CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeItemHeight::instance, mT("Design"));
		dic->addPropertie(&PropertyTypeBackground::instance, mT("Design"));
	}
}
IGUIListBox::~IGUIListBox()
{
}

}
}