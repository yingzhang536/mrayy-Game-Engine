

#include "stdafx.h"
#include "GUISessionSidePanel.h"
#include "GUIElementRegion.h"

namespace mray
{
namespace GUI
{


IMPLEMENT_RTTI(GUISessionSidePanel, IGUIElement);

const core::string GUISessionSidePanel::ElementType = "GUISessionSidePanel";

float GUISessionSidePanel_ShrinkSpeed = 150;

GUISessionSidePanel::GUISessionSidePanel(IGUIManager* m):
IGUIElement(ElementType,m)
{
	m_minWidth = 59;
	m_maxWidth = 150;
	m_state = Shrink;
}
GUISessionSidePanel::~GUISessionSidePanel()
{
}

bool GUISessionSidePanel::OnEvent(Event* e)
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
			m_state=Shrink;
	}
	return false;
}

void GUISessionSidePanel::Update(float dt)
{
	IGUIElement::Update(dt);

#define DECREASE(x,l,v) if(x>(l)){ x-=(v);} if(x<(l)){x=(l);}
#define INCREASE(x,l,v) if(x<(l)){ x+=(v);} if(x>(l)){x=(l);}

	math::vector2d sz = this->GetSize();
	float a= GetAlpha();
	if (m_state == Shrink)
	{
		DECREASE(sz.x, m_minWidth, GUISessionSidePanel_ShrinkSpeed*dt);
		//DECREASE(a, 0.2, dt);
	}

	if (m_state == Expand)
	{
		//INCREASE(a, 1, dt);
		INCREASE(sz.x, m_maxWidth, GUISessionSidePanel_ShrinkSpeed*dt);
	}
	a = 0.2+0.8*((sz.x - m_minWidth) / (m_maxWidth - m_minWidth));
	SetSize(sz);
	SetAlpha(a);
}
void GUISessionSidePanel::Draw(const math::rectf*vp)
{
	video::IVideoDevice* dev= Engine::getInstance().getDevice();

	const GUIElementRegion* r = GetDefaultRegion();

	dev->draw2DRectangle(r->GetRect(), GetColor());
}

IMPLEMENT_ELEMENT_FACTORY(GUISessionSidePanel);

}
}
