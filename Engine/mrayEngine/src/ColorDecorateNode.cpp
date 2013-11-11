#include "stdafx.h"

#include "ColorDecorateNode.h"
#include "StringConverter.h"


namespace mray
{
namespace GUI
{

ColorDecorateNode::ColorDecorateNode():ITextDecorateNode(mT("Color"))
{
}
ColorDecorateNode::~ColorDecorateNode()
{
}

void ColorDecorateNode::ParseParameter(const core::string &p)
{
	m_Color=core::StringConverter::toColor(p);
}

void ColorDecorateNode::OnEnter(TextContextAttributes*context)
{
	m_SavedColor=context->fontAttributes.fontColor;
	context->fontAttributes.fontColor=m_Color;
}
void ColorDecorateNode::OnExit(TextContextAttributes*context)
{
	context->fontAttributes.fontColor=m_SavedColor;
}

}
}


