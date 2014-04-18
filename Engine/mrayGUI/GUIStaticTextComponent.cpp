

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
m_background(true),m_border(true)
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


	if(m_background && skin)
		skin->drawBox(creator->GetRenderQueue(),rc,mT("Panel"),0,m_backColor);
	if(m_border)
		creator->GetDevice()->draw2DRectangle(rc,m_borderColor,0);

	if(font)
	{
		fAttrs->fontColor.A=owner->GetDerivedAlpha();
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