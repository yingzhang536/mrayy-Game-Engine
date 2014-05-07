

#include "stdafx.h"
#include "GUISessionDetailsTopPanel.h"

#include "GUIElementRegion.h"
#include "FontResourceManager.h"
#include "IGUIManager.h"
#include "IGUIRenderer.h"
#include "DateTime.h"
#include "GUIOverlay.h"
#include "GUIOverlayManager.h"
#include "GUISessionSidePanel.h"

#include "CSpeaker.h"
#include "SessionDetails.h"

namespace mray
{
namespace GUI
{



const GUID GUISessionDetailsTopPanel::ElementType = "GUISessionDetailsTopPanel";

float GUISessionDetailsTopPanel_ShrinkSpeed = 150;

GUISessionDetailsTopPanel::GUISessionDetailsTopPanel(IGUIManager* m) :
IGUIPanelElement(ElementType, m)
{
	m_active = false;
	m_sidePanel = 0;

	GUI::GUIOverlay* o= GUI::GUIOverlayManager::getInstance().LoadOverlay("GUISessionDetailsPanelLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
}
GUISessionDetailsTopPanel::~GUISessionDetailsTopPanel()
{
}

void GUISessionDetailsTopPanel::SetSidePanel(GUISessionSidePanel* p)
{
	m_sidePanel = p;
	m_sidePanel->OnSpeakerChange += CreateObjectDelegate(GUISessionDetailsTopPanel, this, _OnSpeakerChange);
}
bool GUISessionDetailsTopPanel::OnEvent(Event* e)
{
	if (e->getType() == ET_Mouse)
	{
		MouseEvent* evt = dynamic_cast<MouseEvent*>(e);
		const GUIElementRegion* r = GetDefaultRegion();
		if (r->GetClippedRect().IsPointInside(evt->pos))
		{
			m_active = true;
		}
		else
			m_active = false;
	}
	return false;
}
void GUISessionDetailsTopPanel::_OnSpeakerChange(IObject* sender, PVOID param)
{
	ted::CSpeaker* speaker= m_sidePanel->GetActiveSpeaker();
	ted::SessionDetails* session= speaker->GetSession();
	SessionName->SetText(session->GetSessionName());
	SpeakerName->SetText(speaker->GetTwitterID());
	SpeakerImage->SetSourceImage(speaker->GetPicture());
}

void GUISessionDetailsTopPanel::Update(float dt)
{
	IGUIPanelElement::Update(dt);

#define DECREASE(x,l,v) if(x>(l)){ x-=(v);} if(x<(l)){x=(l);}
#define INCREASE(x,l,v) if(x<(l)){ x+=(v);} if(x>(l)){x=(l);}

	float a = GetAlpha();
	if (!m_active)
	{
		DECREASE(a, 0.2, dt);
	}else
	{
		INCREASE(a, 1, dt);
	}
	SetAlpha(a);
}
void GUISessionDetailsTopPanel::Draw(const math::rectf*vp)
{
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	const GUIElementRegion* r = GetDefaultRegion();

	dev->draw2DRectangle(r->GetRect(), GetColor());


	core::DateTime dt = core::DateTime::Now();
	SessionTime->SetText(core::DateTime::ToString(dt));

	if (m_sidePanel)
	{
		const math::rectf& rc= m_sidePanel->GetDefaultRegion()->GetClippedRect();
		math::vector2d pos = GetPosition();
		pos.x = rc.BRPoint.x;
		this->SetPosition(pos);
	}
	IGUIPanelElement::Draw(vp);
}

IMPLEMENT_RTTI(GUISessionDetailsTopPanel, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUISessionDetailsTopPanel);

}
}


