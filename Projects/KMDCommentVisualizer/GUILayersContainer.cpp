

#include "stdafx.h"
#include "GUILayersContainer.h"



namespace mray
{
namespace GUI
{

const GUID GUILayersContainer::ElementType = "GUILayersContainer";

IMPLEMENT_RTTI(GUILayersContainer, IGUIPanelElement);
GUILayersContainer::GUILayersContainer(GUI::IGUIManager* m)
	:IGUIPanelElement(ElementType, m), GUIDockedElement(this)
{
}
GUILayersContainer::~GUILayersContainer()
{
}

void GUILayersContainer::Draw(const math::rectf*vp)
{
	math::vector2d sz  = vp->getSize();
	SetSize(sz);
	GUIDockedElement::UpdateDocking();
	IGUIPanelElement::Draw(vp);
}

IMPLEMENT_ELEMENT_FACTORY(GUILayersContainer);

}
}
