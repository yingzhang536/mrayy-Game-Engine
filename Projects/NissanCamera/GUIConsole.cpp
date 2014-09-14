

#include "stdafx.h"
#include "GUIConsole.h"
#include "GUIOverlay.h"
#include "GUIOverlayManager.h"
#include "GUIElementRegion.h"
#include "StringUtil.h"


namespace mray
{
namespace GUI
{



const GUID GUIConsole::ElementType = "GUIConsole";

float GUIConsole_ShrinkSpeed = 150;

GUIConsole::GUIConsole(IGUIManager* m) :
	IGUIPanelElement(ElementType, m)
{
	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUI/GUIConsoleLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);

		Command->OnKeyDown += newClassDelegate2("", this, &GUIConsole::_OnCommand);
		History->OnSelectChange += newClassDelegate2("", this, &GUIConsole::_OnSelectCommand);
	}
}
GUIConsole::~GUIConsole()
{
}

void GUIConsole::_OnCommand(IObject* obj, void* data)
{
	KeyboardEvent* event = (KeyboardEvent*)(data);
	if (event->press && event->key == KEY_RETURN && Command->GetText().Length()>0)
	{
		core::UTFString str = core::UTFString("> ");
		str+=Command->GetText();
		GUI::StringListItem* item = new GUI::StringListItem(str);
		History->AddItem(item);

		core::string cmd = Command->GetText().GetAsString();
		OnCommand(this, cmd);
		Command->SetText(core::string(""));
	}
}

void GUIConsole::_OnSelectCommand(IObject* obj, void* data)
{
	int i=History->GetSelectedItem();
	if (i == -1)
		return;
//	Command->SetText(History->GetItem(i)->toString());
}

bool GUIConsole::OnEvent(Event* e)
{
	if (!IsVisible())
		return false;
	if (e->getType() == ET_Mouse)
	{
		MouseEvent* evt = dynamic_cast<MouseEvent*>(e);
		const GUIElementRegion* r = GetDefaultRegion();
		if (r->GetClippedRect().IsPointInside(evt->pos))
		{
			//return true;
		}
		else	{

		}
	}
	return IGUIPanelElement::OnEvent(e);
}

void GUIConsole::Update(float dt)
{
	if (!IsVisible())
		return;
	IGUIPanelElement::Update(dt);
	return;
#define DECREASE(x,l,v) if(x>(l)){ x-=(v);} if(x<(l)){x=(l);}
#define INCREASE(x,l,v) if(x<(l)){ x+=(v);} if(x>(l)){x=(l);}

	math::vector2d sz = this->GetSize();
	float a = GetAlpha();
	//	if (!m_active)
	{
		DECREASE(sz.y, 120, 200 * dt);
		DECREASE(a, 0.2, dt);
	}/*
		else
		{
		INCREASE(sz.y, 160, 200 * dt);
		INCREASE(a, 1, dt);
		}*/
	SetSize(sz);
	SetAlpha(a);
}
void GUIConsole::Draw(const math::rectf*vp)
{
	if (!IsVisible())
		return;
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	const GUIElementRegion* r = GetDefaultRegion();

	IGUIPanelElement::Draw(vp);
}

void GUIConsole::AddToHistory(const core::string& text, const video::SColor& color)
{
	std::vector<core::string> vals= core::StringUtil::Split(text, "\n");
	for (int i = 0; i < vals.size(); ++i)
	{
		if (vals[i] == "")
			continue;
		GUI::StringListItem* item = new GUI::StringListItem(vals[i]);
		History->AddItem(item);
		History->SetStartItem(History->GetItemsCount() - History->GetItemsPerPage());
	}
}
IMPLEMENT_RTTI(GUIConsole, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUIConsole);

}
}

