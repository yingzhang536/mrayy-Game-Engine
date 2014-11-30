


#include "stdafx.h"
#include "GUIScenarioElement.h"

#include "GUIOverlayManager.h"
#include "GUIOverlay.h"


namespace mray
{
namespace GUI
{

	const GUID GUIScenarioElement::ElementType = "GUIScenarioElement";

GUIScenarioElement::GUIScenarioElement(IGUIManager* m)
	:IGUIPanelElement(ElementType, m)
{
	m_currentScenario = -1;
	m_maxTime = 5000;
	m_startTime = 0;
	m_fadeSpeed = 0.5f;
	m_state = EWait;
	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIScenarioLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
}

GUIScenarioElement::~GUIScenarioElement()
{
}


void GUIScenarioElement::AddScenario(const core::string& image)
{
	ScenarioInfo ifo;
	ifo.image = image;
	m_scenarios.push_back(ifo);
}
void GUIScenarioElement::Reset()
{
	m_state = EWait;
	ScenarioImg->SetAlpha(0);
	m_currentScenario = -1;
}


bool GUIScenarioElement::IsDone()
{
	return m_currentScenario >= (int)m_scenarios.size() || m_scenarios.size() == 0;
}


bool GUIScenarioElement::Next()
{
	ScenarioImg->SetAlpha(0);
	if (m_currentScenario >=(int) m_scenarios.size() || m_scenarios.size() == 0)
		return false;
	m_currentScenario++;
	if (m_currentScenario >= (int)m_scenarios.size())
		return false;

	ScenarioImg->SetSourceImage(m_scenarios[m_currentScenario].image);

	m_state = EStart;
}


void GUIScenarioElement::Update(float dt)
{
	IGUIPanelElement::Update(dt);
	if (IsDone())
		return;
	if (m_state == EStart)
	{
		float a = ScenarioImg->GetAlpha();
		a += dt*m_fadeSpeed;
		a = math::Min(1.0f, a);
		ScenarioImg->SetAlpha(a);
		if (a == 1)
		{
			m_state = EIdle;
			m_startTime = gEngine.getTimer()->getSeconds();
		}
	}
	if (m_state==EIdle)
	{
		if (gEngine.getTimer()->getSeconds() - m_startTime > m_maxTime)
			m_state = EDone;
	}
	if (m_state == EDone)
	{
		float a = ScenarioImg->GetAlpha();
		a -= dt*m_fadeSpeed;
		a = math::Max(0.0f, a);
		ScenarioImg->SetAlpha(a);
		if (a == 0)
		{
			m_state = EWait;
		}
	}
}


void GUIScenarioElement::DoneLoadingElements()
{
}
void GUIScenarioElement::LoadXMLSettings(xml::XMLElement*e)
{
	IGUIPanelElement::LoadXMLSettings(e);
	m_fadeSpeed = e->getValueFloat("FadeSpeed");
	m_maxTime = e->getValueFloat("MaxTime");
	xml::XMLElement* elem = e->getSubElement("Scenario");
	while (elem)
	{
		AddScenario(elem->getValueString("Image"));
		elem = elem->nextSiblingElement("Scenario");
	}

}

IMPLEMENT_RTTI(GUIScenarioElement, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUIScenarioElement);

}
}

