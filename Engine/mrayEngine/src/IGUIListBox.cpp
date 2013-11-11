
#include "stdafx.h"
#include "IGUIListBox.h"

namespace mray
{
namespace GUI
{

const GUID IGUIListBox::ElementType("GUIListBox");

IGUIListBox::IGUIListBox(IGUIManager* manager):IGUIElement(ElementType,manager),
OnKeyDown(mT("KeyDown")),OnSelectChange(mT("SelectChange"))
{
	AddEvent(&OnKeyDown);
	AddEvent(&OnSelectChange);
}
IGUIListBox::~IGUIListBox()
{
}

}
}