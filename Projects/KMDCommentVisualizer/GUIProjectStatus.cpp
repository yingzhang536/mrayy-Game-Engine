

#include "stdafx.h"
#include "GUIProjectStatus.h"

#include "GUIElementRegion.h"
#include "FontResourceManager.h"
#include "IGUIManager.h"
#include "IGUIRenderer.h"
#include "DateTime.h"
#include "GUIOverlay.h"
#include "GUIOverlayManager.h"
#include "GUISessionSidePanel.h"
#include "KMDComment.h"
#include "KMDUser.h"
#include "StringUtil.h"

namespace mray
{
namespace GUI
{



const GUID GUIProjectStatus::ElementType = "GUIProjectStatus";

GUIProjectStatus::GUIProjectStatus(IGUIManager* m) :
	IGUIPanelElement(ElementType, m)
{
	m_project = 0;
	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIProjectStatusLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
}
GUIProjectStatus::~GUIProjectStatus()
{
}

void GUIProjectStatus::SetProject(kmd::SessionDetails* t)
{
	m_project = t;
	if (m_project)
	{
		ProjectImage->SetImage(m_project->GetTexture());
		ProjectName->SetText(m_project->GetSessionName());
		TotalComments->SetText(core::StringConverter::toString(m_project->GetComments().size()));
	}
}
void GUIProjectStatus::Update(float dt)
{
	if (!IsVisible())
		return;
	IGUIPanelElement::Update(dt);
	if (m_project)
	{
		int s=0, f = 0;
		const std::vector<kmd::KMDComment*>& lst = m_project->GetComments();
		for (int i = 0; i < lst.size(); ++i)
		{
			if (lst[i]->user == kmd::KMDUser::studentUser)
				s++;
			else if (lst[i]->user == kmd::KMDUser::facultyUser)
				f++;
		}
		TotalComments->SetText(core::StringConverter::toString(m_project->GetComments().size()));
		StudentsComments->SetText(core::StringConverter::toString(s));
		FaculyComments->SetText(core::StringConverter::toString(f));
	}
	return;
}
void GUIProjectStatus::Draw(const math::rectf*vp)
{
	if (!IsVisible())
		return;
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	const GUIElementRegion* r = GetDefaultRegion();

	//	dev->draw2DRectangle(r->GetClippedRect(), GetColor());


	//	core::DateTime dt = core::DateTime::Now();

	IGUIPanelElement::Draw(vp);
}

IMPLEMENT_RTTI(GUIProjectStatus, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUIProjectStatus);

}
}

