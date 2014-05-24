

#include "stdafx.h"
#include "GUITweetDetailsPanel.h"

#include "GUIElementRegion.h"
#include "FontResourceManager.h"
#include "IGUIManager.h"
#include "IGUIRenderer.h"
#include "DateTime.h"
#include "GUIOverlay.h"
#include "GUIOverlayManager.h"
#include "GUISessionSidePanel.h"
#include "TwitterTweet.h"
#include "TwitterUserProfile.h"

namespace mray
{
namespace GUI
{



	const GUID GUITweetDetailsPanel::ElementType = "GUITweetDetailsPanel";

float GUITweetDetailsPanel_ShrinkSpeed = 150;

GUITweetDetailsPanel::GUITweetDetailsPanel(IGUIManager* m) :
	IGUIPanelElement(ElementType, m)
{
	m_active = false;
	m_sidePanel = 0;
	m_tweet = 0;
	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUITweetDetailsPanelLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
}
GUITweetDetailsPanel::~GUITweetDetailsPanel()
{
}

void GUITweetDetailsPanel::SetTweet(ted::TwitterTweet* t)
{
	m_tweet = t;
	if (m_tweet)
	{
		TwitterID->SetText(t->user->name);
		Details->SetText(t->text);
		TwitterImage->SetSourceImage(t->user->imageUrl);
		TweetTime->SetText(core::DateTime::ToString(t->date));
		m_active = true;
	}
	else
		m_active = false;
}
bool GUITweetDetailsPanel::OnEvent(Event* e)
{
	if (e->getType() == ET_Mouse)
	{
		MouseEvent* evt = dynamic_cast<MouseEvent*>(e);
		const GUIElementRegion* r = GetDefaultRegion();
		if (r->GetClippedRect().IsPointInside(evt->pos))
		{
			m_active = true;
			return true;
		}
		else	{

			m_active = false;
		}
	}
	return false;
}

void GUITweetDetailsPanel::Update(float dt)
{
	IGUIPanelElement::Update(dt);

#define DECREASE(x,l,v) if(x>(l)){ x-=(v);} if(x<(l)){x=(l);}
#define INCREASE(x,l,v) if(x<(l)){ x+=(v);} if(x>(l)){x=(l);}

	math::vector2d sz = this->GetSize();
	float a = GetAlpha();
	if (!m_active)
	{
		DECREASE(sz.y, 100, 200*dt);
		DECREASE(a, 0.2, dt);
	}
	else
	{
		INCREASE(sz.y, 200, 200 * dt);
		INCREASE(a, 1, dt);
	}
	SetSize(sz);
	SetAlpha(a);
}
void GUITweetDetailsPanel::Draw(const math::rectf*vp)
{
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	const GUIElementRegion* r = GetDefaultRegion();

	dev->draw2DRectangle(r->GetRect(), GetColor());


//	core::DateTime dt = core::DateTime::Now();

	if (m_sidePanel)
	{
		const math::rectf& rc = m_sidePanel->GetDefaultRegion()->GetClippedRect();
		math::vector2d pos = GetPosition();
		pos.x = rc.BRPoint.x;
		this->SetPosition(pos);
	}
	IGUIPanelElement::Draw(vp);
}

IMPLEMENT_RTTI(GUITweetDetailsPanel, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUITweetDetailsPanel);

}
}


