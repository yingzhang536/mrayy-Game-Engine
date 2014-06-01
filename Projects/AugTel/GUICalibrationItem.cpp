
#include "stdafx.h"
#include "GUICalibrationItem.h"

#include "GUIElementRegion.h"

#include "GUIOverlayManager.h"
#include "GUIOverlay.h"

namespace mray
{
namespace GUI
{
const GUID GUICalibrationItem::ElementType = "GUICalibrationItem";


GUICalibrationItem::GUICalibrationItem(IGUIManager* m)
	:IGUIPanelElement(ElementType, m)
{

	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUICalibrationItemLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}

}

GUICalibrationItem::~GUICalibrationItem(){
}



IMPLEMENT_RTTI(GUICalibrationItem, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUICalibrationItem);

}
}
