
#include "stdafx.h"
#include "GUIFadingText.h"
#include "GUIElementRegion.h"
#include "IGUIManager.h"
#include "IGUIRenderer.h"
#include "FontResourceManager.h"

namespace mray
{
namespace GUI
{



const GUID GUIFadingText::ElementType("GUIFadingText");

GUIFadingText::GUIFadingText(GUI::IGUIManager* creator)
	:IGUIElement(ElementType, creator)
{
	m_targetAlpha = 1;
	m_time = 0;
}
GUIFadingText::~GUIFadingText()
{

}

bool GUIFadingText::SetText(const core::UTFString&t)
{
	m_targetText = t;
	SetAlpha(0);
	m_time = 0;
	return IGUIElement::SetText(core::UTFString::Empty);
}

void GUIFadingText::Update(float dt)
{
	float a = GetAlpha();
	a = math::Min(m_targetAlpha, a + dt*0.1f);
	SetAlpha(a);
	m_time += dt;

	int len = m_time * 300;
	if (len >= m_targetText.Length())
		len = m_targetText.Length() - 1;

	IGUIElement::SetText(m_targetText.substr(0, len));
}
void GUIFadingText::Draw(const math::rectf*vp)
{

	if (!IsVisible())
		return;

	math::rectf rc = GetDefaultRegion()->GetClippedRect();
	math::rectf oldScissor = gEngine.getDevice()->getScissorRect();
	//	gEngine.getDevice()->setScissorRect(rc);
	IFont*font = gFontResourceManager.getFontByName(m_fontAttrs.fontName);

	if (font)
	{
		m_fontAttrs.fontColor.A = GetDerivedAlpha();
		m_fontAttrs.shadowColor.A = m_fontAttrs.fontColor.A;
		font->print(rc, &m_fontAttrs, &rc, GetText(), GetCreator()->GetRenderQueue());


	}

	GetCreator()->GetRenderQueue()->Flush();
	gEngine.getDevice()->setScissorRect(oldScissor);
}


IMPLEMENT_RTTI(GUIFadingText, IGUIElement);
IMPLEMENT_ELEMENT_FACTORY(GUIFadingText);

}
}
