

#include "stdafx.h"
#include "GUINavigationElement.h"
#include "GUIOverlayManager.h"
#include "GUIOverlay.h"



namespace mray
{
namespace GUI
{

const GUID GUINavigationElement::ElementType = "GUINavigationElement";

GUINavigationElement::GUINavigationElement(IGUIManager* m)
	:IGUIPanelElement(ElementType,m)
{
	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUINavigationLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
}

GUINavigationElement::~GUINavigationElement()
{
}


void GUINavigationElement::SetSpeed(float x, float y, float rotation, float pan, float tilt)
{
	m_target.speed.set(x, y);
	m_target.rotation = rotation;
	m_target.head.set(pan, tilt);


	m_current = m_target;
}


void GUINavigationElement::Update(float dt)
{
	IGUIPanelElement::Update(dt);
	//m_current.speed += (m_target.speed - m_current.speed)*dt;

	float xFactor = sqrt(fabs(m_current.speed.x));
	float yFactor = sqrt(fabs(m_current.rotation));

	IGUIElement* xElem=0;
	IGUIElement* yElem=0;
	float xRef, yRef;
	if (m_current.speed.x < 0)
	{
		BackArrow->SetVisible(false);
		xElem = FrontArrow;
		xRef = m_frontPos;
	}
	else if (m_current.speed.x > 0)
	{
		FrontArrow->SetVisible(false);
		xElem = BackArrow;
		xRef = m_backPos;
	}
	else
	{
		FrontArrow->SetVisible(false);
		BackArrow->SetVisible(false);
	}

	if (m_current.rotation > 0)
	{
		LeftArrow->SetVisible(false);
		yElem = RightArrow;
		yRef = m_rightPos;
	}
	else if (m_current.rotation <0)
	{
		RightArrow->SetVisible(false);
		yElem = LeftArrow;
		yRef = m_leftPos;
	}
	else
	{
		RightArrow->SetVisible(false);
		LeftArrow->SetVisible(false);
	}

	if (xElem)
	{
		xElem->SetVisible(true);
		xElem->SetAlpha(xFactor);

		math::vector2d pos = xElem->GetPosition();
		pos.y = xRef + xFactor * 100 * math::sign(m_current.speed.x);
		xElem->SetPosition(pos);
	}
	if (yElem)
	{
		yElem->SetVisible(true);
		yElem->SetAlpha(yFactor);
		math::vector2d pos = yElem->GetPosition();
		pos.x = yRef + yFactor * 150 * math::sign(m_current.rotation);
		yElem->SetPosition(pos);
	}

	float hFact=math::sind(m_current.head.x);
	float vFact = math::sind(m_current.head.y);

	math::vector2d hPos= HorArrow->GetPosition();
	hPos.x = -hFact * 500;
	HorArrow->SetPosition(hPos);
	HorArrow->SetAlpha(fabs(hFact));

	math::vector2d vPos = VerArrow->GetPosition();
	vPos.y = -vFact * 500;
	VerArrow->SetPosition(vPos);
	VerArrow->SetAlpha(fabs(vFact));
}
void GUINavigationElement::DoneLoadingElements()
{
	m_leftPos = LeftArrow->GetPosition().x;
	m_rightPos = RightArrow->GetPosition().x;
	m_frontPos = FrontArrow->GetPosition().y;
	m_backPos = BackArrow->GetPosition().y;
}

IMPLEMENT_RTTI(GUINavigationElement, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUINavigationElement);

}
}
