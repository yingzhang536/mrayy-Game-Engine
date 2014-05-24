

#include "stdafx.h"
#include "NodeHeadController.h"
#include "IMovable.h"


namespace mray
{
namespace TBee
{
NodeHeadController::NodeHeadController()
{
	m_node = 0;
}
NodeHeadController::~NodeHeadController()
{
}

math::quaternion NodeHeadController::GetHeadOrientation()
{
	if (m_node)
		return m_node->getAbsoluteOrintation()*m_initialOrientation.inverse();
	return math::quaternion::Identity;
}
math::vector3d NodeHeadController::GetHeadPosition()
{
	if (m_node)
		return m_node->getAbsolutePosition();
	return 0;
}

void NodeHeadController::Recalibrate()
{
	if (m_node)
		m_initialOrientation = m_node->getAbsoluteOrintation();
}

}
}

