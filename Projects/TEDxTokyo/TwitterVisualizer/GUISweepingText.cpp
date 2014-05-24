
#include "stdafx.h"
#include "GUISweepingText.h"
#include "GUIElementRegion.h"
#include "IGUIManager.h"
#include "IGUIRenderer.h"
#include "FontResourceManager.h"

namespace mray
{
namespace GUI
{



const GUID GUISweepingText::ElementType("GUISweepingText");

GUISweepingText::GUISweepingText(GUI::IGUIManager* creator)
	:IGUIElement(ElementType,creator)
{
	m_maxTime = 1;
	m_time=0;
}
GUISweepingText::~GUISweepingText()
{

}

bool GUISweepingText::SetText(const core::UTFString&t)
{
	PastTextIfo ifo;
	if (GetText().GetAsString() != "" && GetText().GetAsString() != t.GetAsString())
	{
		ifo.text = GetText();
		m_pastText.push_back(ifo);
		m_time = 0;
	}
	return IGUIElement::SetText(t);
}

void GUISweepingText::Update(float dt)
{
	m_time += dt;
	if (m_time > m_maxTime)
		m_time = m_maxTime;
	std::list<PastTextIfo>::iterator it= m_pastText.begin();
	for (; it != m_pastText.end();)
	{
		(*it).time += dt;
		if ((*it).time > m_maxTime)
		{
			std::list<PastTextIfo>::iterator it2 = it;
			++it;
			m_pastText.erase(it2);
		}
		else
			++it;
	}
}
void GUISweepingText::Draw(const math::rectf*vp)
{

	if (!IsVisible())
		return;

	math::rectf rc = GetDefaultRegion()->GetClippedRect();
	math::rectf oldScissor = gEngine.getDevice()->getScissorRect();
//	gEngine.getDevice()->setScissorRect(rc);
	IFont*font = gFontResourceManager.getFontByName(m_fontAttrs.fontName);

	if (font)
	{
		math::rectf rc2 = rc;
		float a = m_time / m_maxTime;
		float x = 200 * (1 - m_time / m_maxTime);

		rc2.ULPoint.x = rc.ULPoint.x + x;
		rc2.BRPoint.x = rc.BRPoint.x + x;
		m_fontAttrs.fontColor.A = GetDerivedAlpha()*a;
		m_fontAttrs.shadowColor.A = m_fontAttrs.fontColor.A;
		font->print(rc2, &m_fontAttrs, &rc2, GetText(), GetCreator()->GetRenderQueue());

		std::list<PastTextIfo>::iterator it = m_pastText.begin();
		for (; it != m_pastText.end();++it)
		{
			PastTextIfo& ifo = *it;
			 a = 1.0f-ifo.time / m_maxTime;
			x = -200* ifo.time / m_maxTime ;

			rc2.ULPoint.x = rc.ULPoint.x + x;
			rc2.BRPoint.x = rc.BRPoint.x + x;
			m_fontAttrs.fontColor.A = GetDerivedAlpha()*a;
			m_fontAttrs.shadowColor.A = m_fontAttrs.fontColor.A;
			font->print(rc2, &m_fontAttrs, &rc2, ifo.text, GetCreator()->GetRenderQueue());
		}
		
	}

	GetCreator()->GetRenderQueue()->Flush();
	gEngine.getDevice()->setScissorRect(oldScissor);
}


IMPLEMENT_RTTI(GUISweepingText, IGUIElement);
IMPLEMENT_ELEMENT_FACTORY(GUISweepingText);

}
}
