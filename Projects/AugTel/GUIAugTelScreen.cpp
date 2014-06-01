

#include "stdafx.h"
#include "GUIAugTelScreen.h"

#include "GUIElementRegion.h"

#include "GUIOverlayManager.h"
#include "GUIOverlay.h"

namespace mray
{
namespace GUI
{

const GUID GUIAugTelScreen::ElementType = "GUIAugTelScreen";


GUIAugTelScreen::GUIAugTelScreen(IGUIManager* m)
	:IGUIPanelElement(ElementType, m)
{

	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIAugTelScreenLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}

}

GUIAugTelScreen::~GUIAugTelScreen(){
}


void GUIAugTelScreen::OnDisconnected()
{
	WaitPanel->SetVisible(true);
}
void GUIAugTelScreen::OnConnected()
{
	WaitPanel->SetVisible(false);
}

IMPLEMENT_RTTI(GUIAugTelScreen, IGUIPanelElement);

}
}
