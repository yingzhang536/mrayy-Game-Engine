

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

#include "CSubProject.h"
#include "SessionDetails.h"
#include "StringUtil.h"

namespace mray
{
namespace GUI
{



const GUID GUISpeakerDetailsPanel::ElementType = "GUISpeakerDetailsPanel";

float GUISpeakerDetailsPanel_ShrinkSpeed = 150;

GUISpeakerDetailsPanel::GUISpeakerDetailsPanel(IGUIManager* m) :
IGUIPanelElement(ElementType, m), GUIDockedElement(this)
{
	m_active = false;

	GUI::GUIOverlay* o= GUI::GUIOverlayManager::getInstance().LoadOverlay("GUISpeakerDetailsPanelLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
	gAppData.subProjectChange.AddListener(this);
}
GUISpeakerDetailsPanel::~GUISpeakerDetailsPanel()
{
	gAppData.subProjectChange.RemoveListener(this);
}

void GUISpeakerDetailsPanel::SetSidePanel(GUISessionSidePanel* p)
{
	this->SetDockLeft(p);
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

	UpdateDocking();
	IGUIPanelElement::Draw(vp);
}

void GUISpeakerDetailsPanel::_OnSubProjectChange(kmd::CSubProject* sp)
{

	kmd::SessionDetails* session = sp->GetSession();
	SessionName->SetText(core::StringUtil::ToUpper(session->GetSessionName()));
	//core::string txt = core::string("Time Slot ") + core::StringConverter::toString(sp->GetOrder() + 1);
	SpeakerID->SetText(session->GetTheme());
	SpeakerName->SetText(core::StringUtil::ToUpper(sp->GetName()));
	SpeakerTitle->SetText(core::StringUtil::ToUpper(sp->GetTitle()));
	SpeakerImage->SetSourceImage(sp->GetSession()->GetPicture());
	SessionStartTime->SetText(core::CTime::ToString(session->GetSessionStartTime(), false, false));
	SessionEndTime->SetText(core::CTime::ToString(session->GetSessionEndTime(), false, false));
}

IMPLEMENT_RTTI(GUISpeakerDetailsPanel, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUISpeakerDetailsPanel);

}
}


