

#include "stdafx.h"
#include "GUITypingTextBox.h"
#include "GUIElementRegion.h"
#include "IGUIManager.h"
#include "IVideoDevice.h"
#include "FontResourceManager.h"
#include "IGUIRenderer.h"



namespace mray
{
namespace GUI
{

IMPLEMENT_RTTI(GUITypingTextBox, IGUIElement)

IMPLEMENT_SETGET_PROP(GUITypingTextBox, TypingSpeed, float, m_typeSpeed, 10, core::StringConverter::toFloat, core::StringConverter::toString)
IMPLEMENT_SETGET_PROP(GUITypingTextBox, MinChar, int, m_minChar, 0, core::StringConverter::toInt, core::StringConverter::toString)

	const GUID GUITypingTextBox::ElementType("GUITypingTextBox");

GUITypingTextBox::GUITypingTextBox(IGUIManager* mngr)
	:IGUIElement(ElementType, mngr)
{
	m_typeSpeed = 10;
	m_charCount = 0;
	m_minChar = 0;
	CPropertieDictionary *dic = 0;
	if (CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeTypingSpeed::instance, mT("Design"));
		dic->addPropertie(&PropertyTypeMinChar::instance, mT("Design"));
	}
}

GUITypingTextBox::~GUITypingTextBox()
{
}



void GUITypingTextBox::Draw(const math::rectf*vp)
{
	if (!IsVisible())return;
	IGUIManager* creator = GetCreator();
	const math::rectf& rect = GetDefaultRegion()->GetRect();
	const math::rectf& clip = GetDefaultRegion()->GetClippedRect();
	video::IVideoDevice*device = creator->GetDevice();

	IFont* font = gFontResourceManager.getFontByName(m_fontAttrs.fontName);
	if (!font)
		font = gFontResourceManager.getDefaultFont();
	
	math::rectf oldScissor = GetCreator()->GetDevice()->getScissorRect();
	GetCreator()->GetDevice()->setScissorRect(rect);
	int mc = m_charCount + math::Min<int>(m_minChar, GetText().Length());
	mc = math::Min<int>(mc, GetText().Length());
	core::UTFString str = GetText().substr(0,  mc);
	if (font)
	{
// 		m_fontAttrs.fontColor.A = fabs(math::sind(m_time*m_blinkSpeed))*0.8 + 0.2;
// 		m_fontAttrs.shadowColor.A = fabs(math::sind(m_time*m_blinkSpeed))*0.8 + 0.2;
		font->print(rect, &m_fontAttrs, &clip, str, creator->GetRenderQueue());
	}
	GetCreator()->GetRenderQueue()->Flush();
	GetCreator()->GetDevice()->setScissorRect(oldScissor);

	IGUIElement::Draw(vp);
}

void GUITypingTextBox::Update(float dt)
{
	IGUIElement::Update(dt);
	if (!GetText().Length())
		return;

	int mc = math::Min<int>( m_minChar,GetText().Length());
	m_charCount += m_typeSpeed*dt;
	while (m_charCount > m_typeSpeed+GetText().Length() - mc)
		m_charCount -= m_typeSpeed + GetText().Length() - mc;
	
	
}



FontAttributes* GUITypingTextBox::GetFontAttributes()
{
	return &m_fontAttrs;
}

IMPLEMENT_ELEMENT_FACTORY(GUITypingTextBox);


}
}




