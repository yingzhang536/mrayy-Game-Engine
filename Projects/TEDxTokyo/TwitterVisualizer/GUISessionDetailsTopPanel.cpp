

#include "stdafx.h"
#include "GUISessionDetailsTopPanel.h"

#include "GUIElementRegion.h"
#include "FontResourceManager.h"
#include "IGUIManager.h"
#include "IGUIRenderer.h"
#include "DateTime.h"
#include "GUIOverlay.h"
#include "GUIOverlayManager.h"

namespace mray
{
namespace GUI
{


IMPLEMENT_RTTI(GUISessionDetailsTopPanel, IGUIPanelElement);

const core::string GUISessionDetailsTopPanel::ElementType = "GUISessionDetailsTopPanel";

float GUISessionDetailsTopPanel_ShrinkSpeed = 150;

GUISessionDetailsTopPanel::GUISessionDetailsTopPanel(IGUIManager* m) :
IGUIPanelElement(ElementType, m)
{
	m_minWidth = 59;
	m_maxWidth = 150;
	m_state = Shrink;

	GUI::GUIOverlay* o= GUI::GUIOverlayManager::getInstance().LoadOverlay("GUISessionDetailsPanelLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
}
GUISessionDetailsTopPanel::~GUISessionDetailsTopPanel()
{
}

bool GUISessionDetailsTopPanel::OnEvent(Event* e)
{
	if (e->getType() == ET_Mouse)
	{
		MouseEvent* evt = dynamic_cast<MouseEvent*>(e);
		const GUIElementRegion* r = GetDefaultRegion();
		if (r->GetClippedRect().IsPointInside(evt->pos))
		{
			m_state = Expand;
		}
		else
			m_state = Shrink;
	}
	return false;
}

void GUISessionDetailsTopPanel::Update(float dt)
{
	IGUIPanelElement::Update(dt);

#define DECREASE(x,l,v) if(x>(l)){ x-=(v);} if(x<(l)){x=(l);}
#define INCREASE(x,l,v) if(x<(l)){ x+=(v);} if(x>(l)){x=(l);}

	math::vector2d sz = this->GetSize();
	float a = GetAlpha();
	if (m_state == Shrink)
	{
		DECREASE(sz.y, m_minWidth, GUISessionDetailsTopPanel_ShrinkSpeed*dt);
		//DECREASE(a, 0.2, dt);
	}

	if (m_state == Expand)
	{
		//INCREASE(a, 1, dt);
		INCREASE(sz.y, m_maxWidth, GUISessionDetailsTopPanel_ShrinkSpeed*dt);
	}
	a = 0.2 + 0.8*((sz.y - m_minWidth) / (m_maxWidth - m_minWidth));
	SetSize(sz);
	SetAlpha(a);
}
void GUISessionDetailsTopPanel::Draw(const math::rectf*vp)
{
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	const GUIElementRegion* r = GetDefaultRegion();

	dev->draw2DRectangle(r->GetRect(), GetColor());


	core::DateTime dt = core::DateTime::Now();
	SessionTime->SetText(core::DateTime::ToString(dt));

	IGUIPanelElement::Draw(vp);
}

IMPLEMENT_ELEMENT_FACTORY(GUISessionDetailsTopPanel);

}
}


