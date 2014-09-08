
#include "stdafx.h"
#include "GUILoadingScreen.h"
#include "GUIElementRegion.h"

#include "GUIOverlayManager.h"
#include "GUIOverlay.h"

namespace mray
{
namespace GUI
{

const GUID GUILoadingScreen::ElementType = "GUILoadingScreen";


GUILoadingScreen::GUILoadingScreen(IGUIManager* m)
	:IGUIPanelElement(ElementType, m)
{

	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUILoadingScreenLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}

}

GUILoadingScreen::~GUILoadingScreen(){
}
void GUILoadingScreen::SetText(const core::string& text,bool waitText)
{
	core::string str=text;
	if (waitText)
		str += ".........";
	WaitTxt->SetText(str);
	WaitTxt->SetMinChar(text.length());
}
void GUILoadingScreen::SetInfo(const core::string& st)
{
	InfoTxt->SetText(st);
}

void GUILoadingScreen::Reset()
{
	Status->ClearItems();
}
void GUILoadingScreen::AddStatus(const core::string& st)
{
	GUI::StringListItem *itm = new GUI::StringListItem(st);
	Status->AddItem(itm);
}
IMPLEMENT_RTTI(GUILoadingScreen, IGUIPanelElement);

}
}


