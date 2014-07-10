
#include "stdafx.h"
#include "GUIDockedElement.h"
#include "IGUIElement.h"
#include "GUIElementRegion.h"

namespace mray
{
namespace GUI
{
GUIDockedElement::GUIDockedElement(IGUIElement* target)
:m_target(target)
{
	m_left = m_right = m_top = m_bottom = 0;
}
GUIDockedElement::~GUIDockedElement(){}

void GUIDockedElement::UpdateDocking()
{
	if (!m_target)
		return;
	math::vector2d pos = m_target->GetPosition();
	math::vector2d sz = m_target->GetSize();
	if (m_left)
	{
		const math::rectf& rc = m_left->GetDefaultRegion()->GetClippedRect();
		pos.x = rc.BRPoint.x;
	}
	if (m_right)
	{
		const math::rectf& rc = m_right->GetDefaultRegion()->GetClippedRect();
		sz.x = rc.BRPoint.x-pos.x;
	}

	if (m_top)
	{
		const math::rectf& rc = m_top->GetDefaultRegion()->GetClippedRect();
		pos.y = rc.BRPoint.y;
	}

	if (m_bottom)
	{
		const math::rectf& rc = m_bottom->GetDefaultRegion()->GetClippedRect();
		sz.y = rc.BRPoint.y - pos.y;
	}

	m_target->SetPosition(pos);
	m_target->SetSize(sz);
}

}
}
