

#include "stdafx.h"
#include "GUIStatusLayer.h"

#include "GUIOverlayManager.h"
#include "GUIOverlay.h"
#include "GUICommentDetailsPanel.h"
#include "GUIProjectStatus.h"

namespace mray
{
namespace GUI
{

const GUID GUIStatusLayer::ElementType = "GUIStatusLayer";

IMPLEMENT_RTTI(GUIStatusLayer, IGUIPanelElement);

GUIStatusLayer::GUIStatusLayer(GUI::IGUIManager* m)
	:IGUIPanelElement(ElementType, m), GUIDockedElement(this)
{

	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIStatusLayerLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}

}
GUIStatusLayer::~GUIStatusLayer()
{

}

void GUIStatusLayer::SetProjects(kmd::SessionContainer* sessions)
{
	const std::vector<kmd::SessionDetails*>& lst = sessions->GetSessions();
	std::vector<kmd::SessionDetails*>::const_iterator it = lst.begin();
	float y = 20;
	float h = 80;
	for (; it != lst.end();++it)
	{
		GUIProjectStatus* p = new GUIProjectStatus(GetCreator());
		p->SetPosition(math::vector2d(20, y));
		p->SetSize(math::vector2d(0, h));
		p->SetAnchorPosition(math::vector2d(20, 0));
		p->SetAnchorLeft(true);
		p->SetAnchorTop(true);
		p->SetAnchorRight(true);
		p->SetProject(*it);
		AddElement(p);
		y += h;
	}
}

void GUIStatusLayer::Draw(const math::rectf*vp)
{
	math::vector2d sz = GetSize();
	sz.x = vp->getSize().x;
	SetSize(sz);
	GUIDockedElement::UpdateDocking();
	IGUIPanelElement::Draw(vp);
}


void GUIStatusLayer::Update(float dt)
{
	IGUIPanelElement::Update(dt);
}

IMPLEMENT_ELEMENT_FACTORY(GUIStatusLayer);

}
}

