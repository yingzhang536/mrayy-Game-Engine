

#include "stdafx.h"
#include "GUIStaticTextComponent.h"
#include "GUIThemeManager.h"
#include "IGUITheme.h"
#include "GUIManager.h"
#include "IGUIElement.h"



namespace mray
{
namespace GUI
{

GUIStaticTextComponent::GUIStaticTextComponent():m_backColor(0.9,0.9,0.9,1),
m_background(true),m_border(false)
{
}
GUIStaticTextComponent::~GUIStaticTextComponent()
{
}

void GUIStaticTextComponent::Draw()
{
	IGUIManager* creator=owner->GetCreator();
	IGUITheme* skin=creator->GetActiveTheme();
	FontAttributes* fAttrs=owner->GetFontAttributes();

	IFont*font=gFontResourceManager.getFontByName(fAttrs->fontName);

	float a = owner->GetDerivedAlpha();

	if (m_background)
	{
		if (skin)
			skin->drawBox(creator->GetRenderQueue(), rc, mT("Panel"), 0, video::SColor(m_backColor.R, m_backColor.G, m_backColor.B, a));
		else
			creator->GetDevice()->draw2DRectangle(rc, video::SColor(m_backColor.R, m_backColor.G, m_backColor.B, a));
	}
	if (m_border)
		creator->GetDevice()->draw2DRectangle(rc, video::SColor(m_borderColor.R, m_borderColor.G, m_borderColor.B, a), 0);

	if(font)
	{
		fAttrs->fontColor.A=a;
		fAttrs->shadowColor.A=fAttrs->fontColor.A;
		video::SColor oldColor=fAttrs->fontColor;
		if(!enabled)
			fAttrs->fontColor.Set(0.5,0.5,0.5,oldColor.A);
		font->print(rc,fAttrs,&rc,text,creator->GetRenderQueue());
		fAttrs->fontColor=oldColor;
	}
}

bool GUIStaticTextComponent::OnKeyboardEvent(KeyboardEvent* e)
{
	return true;
}
bool GUIStaticTextComponent::OnMouseEvent(MouseEvent* e)
{
	return true;
}


}
}