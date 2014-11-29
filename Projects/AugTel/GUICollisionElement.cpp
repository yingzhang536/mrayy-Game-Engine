

#include "stdafx.h"
#include "GUICollisionElement.h"

#include "GUIOverlayManager.h"
#include "GUIOverlay.h"



namespace mray
{
namespace GUI
{

const GUID GUICollisionElement::ElementType = "GUICollisionElement";

GUICollisionElement::GUICollisionElement(IGUIManager* m)
:IGUIPanelElement(ElementType, m)
{
	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUICollisionLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
	for (int i = 0; i < 6; ++i)
		m_bumpers[i] = 0;
}

GUICollisionElement::~GUICollisionElement()
{
}
void GUICollisionElement::SetSensors(int count, float *v)
{
	float vals[6];
	float step = (float)count / 6.0f;
	for (int i = 0; i < 6; ++i)
	{
		if (m_bumpers[i] != 0)
			continue;
		vals[i] = v[(int)floor(i*step)];
		vals[i] = math::Min(vals[i] / 256.0f, 1.0f);
		vals[i] = sqrt(vals[i]);
		m_indicators[i]->SetAlpha(vals[i]);
	}
}
void GUICollisionElement::SetBumper(int count, bool *v)
{
	float step = (float)count / 6.0f;
	for (int i = 0; i < 6; ++i)
	{
		m_bumpers[i] = v[(int)floor(i*step)] == true ? 1.0f : 0.0f;
		m_indicators[i]->SetAlpha(m_bumpers[i]);
	}
}


void GUICollisionElement::Update(float dt)
{
	IGUIPanelElement::Update(dt);

}


void GUICollisionElement::DoneLoadingElements()
{
	m_indicators[0] = Left;
	m_indicators[1] = LeftFront;
	m_indicators[2] = LeftCenter;
	m_indicators[3] = RightCenter;
	m_indicators[4] = RightFront;
	m_indicators[5] = Right;

	for (int i = 0; i < 6; ++i)
	{
		m_indicators[i]->SetAlpha(0);
		m_indicators[i]->SetColor(video::SColor(1, 0, 0, 0));
	}
}

IMPLEMENT_RTTI(GUICollisionElement, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUICollisionElement);

}
}