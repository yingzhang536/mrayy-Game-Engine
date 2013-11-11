#include "stdafx.h"


#include "FontDecorateFont.h"
#include "StringConverter.h"
#include "FontResourceManager.h"


namespace mray
{
namespace GUI
{

FontDecorateFont::FontDecorateFont():ITextDecorateNode(mT("Font"))
{
	m_Font=0;
	m_SavedFont=0;
}
FontDecorateFont::~FontDecorateFont()
{
}

void FontDecorateFont::ParseParameter(const core::string &p)
{
	m_Font=gFontResourceManager.getFontByName(p);
}

void FontDecorateFont::OnEnter(TextContextAttributes*context)
{
	m_SavedFont=context->font;
	context->font=m_Font;
}
void FontDecorateFont::OnExit(TextContextAttributes*context)
{
	context->font=m_SavedFont;
}

}
}



