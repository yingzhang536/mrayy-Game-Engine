#include "stdafx.h"

#include "SizeDecorateNode.h"
#include "StringConverter.h"

namespace mray
{
namespace GUI
{

SizeDecorateNode::SizeDecorateNode():ITextDecorateNode(mT("Size"))
{
	m_Size=10;
	m_SavedSize=0;
}
SizeDecorateNode::~SizeDecorateNode()
{
}

void SizeDecorateNode::ParseParameter(const core::string &p)
{
	m_Size=core::StringConverter::toFloat(p);
}

void SizeDecorateNode::OnEnter(TextContextAttributes*context)
{
	m_SavedSize=context->fontAttributes.fontSize;
	context->fontAttributes.fontSize=m_Size;
}
void SizeDecorateNode::OnExit(TextContextAttributes*context)
{
	context->fontAttributes.fontSize=m_SavedSize;
}

}
}


