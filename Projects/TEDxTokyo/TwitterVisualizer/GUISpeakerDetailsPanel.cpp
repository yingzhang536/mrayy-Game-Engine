

#include "stdafx.h"
#include "GUISpeakerDetailsPanel.h"

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
#include "StringUtil.h"

namespace mray
{
namespace GUI
{



const GUID GUISpeakerDetailsPanel::ElementType = "GUISpeakerDetailsPanel";

float GUISpeakerDetailsPanel_ShrinkSpeed = 150;

GUISpeakerDetailsPanel::GUISpeakerDetailsPanel(IGUIManager* m) :
IGUIPanelElement(ElementType, m)
{
	m_active = false;
	m_sidePanel = 0;

	GUI::GUIOverlay* o= GUI::GUIOverlayManager::getInstance().LoadOverlay("GUISpeakerDetailsPanelLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
}
GUISpeakerDetailsPanel::~GUISpeakerDetailsPanel()
{
}

void GUISpeakerDetailsPanel::SetSidePanel(GUISessionSidePanel* p)
{
	m_sidePanel = p;
	m_sidePanel->OnSpeakerChange += CreateObjectDelegate(GUISpeakerDetailsPanel, this, _OnSpeakerChange);
}
bool GUISpeakerDetailsPanel::OnEvent(Event* e)
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
void GUISpeakerDetailsPanel::_OnSpeakerChange(IObject* sender, PVOID param)
{
	ted::CSpeaker* speaker= m_sidePanel->GetActiveSpeaker();
	ted::SessionDetails* session= speaker->GetSession();
	SessionName->SetText(core::StringUtil::ToUpper(session->GetSessionName()));
	core::string sp = core::string("SPEAKER ") + core::StringConverter::toString(speaker->GetOrder()+1);
	SpeakerID->SetText(sp);
	SpeakerName->SetText( core::StringUtil::ToUpper(speaker->GetName()));
	SpeakerTitle->SetText(core::StringUtil::ToUpper(speaker->GetTitle()));
	TwitterID->SetText("#"+speaker->GetTwitterID());
	SpeakerImage->SetSourceImage(speaker->GetPicture());
	SessionStartTime->SetText(core::CTime::ToString(session->GetSessionStartTime(), false, false));
	SessionEndTime->SetText(core::CTime::ToString(session->GetSessionEndTime(), false, false));

}

void GUISpeakerDetailsPanel::Update(float dt)
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
void GUISpeakerDetailsPanel::Draw(const math::rectf*vp)
{
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	const GUIElementRegion* r = GetDefaultRegion();

	dev->draw2DRectangle(r->GetRect(), GetColor());


	//core::DateTime dt = core::DateTime::Now();
	//SessionTime->SetText(core::DateTime::ToString(dt));

	if (m_sidePanel)
	{
		const math::rectf& rc= m_sidePanel->GetDefaultRegion()->GetClippedRect();
		math::vector2d pos = GetPosition();
		pos.x = rc.BRPoint.x;
		this->SetPosition(pos);
	}
	IGUIPanelElement::Draw(vp);
}

IMPLEMENT_RTTI(GUISpeakerDetailsPanel, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUISpeakerDetailsPanel);

}
}


