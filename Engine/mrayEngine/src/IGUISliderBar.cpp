
#include "stdafx.h"
#include "IGUISliderBar.h"


namespace mray
{
namespace GUI
{

const GUID IGUISliderBar::ElementType("GUISliderBar");

IGUISliderBar::IGUISliderBar(IGUIManager* creator):IGUIElement(ElementType,creator),
OnChanged(mT("OnChanged"))
{
	AddEvent(&OnChanged);
}

IGUISliderBar::~IGUISliderBar()
{
}

}
}

