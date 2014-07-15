

#include "stdafx.h"
#include "PhNode.h"



namespace mray
{
namespace physics
{


PhNode::PhNode()
{
	m_mass = 1;
	m_radius = 1;
	m_fixed = false;
	m_drag = 0;
}

PhNode::~PhNode()
{
}


void PhNode::AddForce(const math::vector2d& v)
{
	m_force += v;
}


void PhNode::Update(float dt)
{
	if (m_fixed)
	{
		m_force = 0;
		return;
	}
	m_position += m_velocity*dt;
	m_force -= m_drag*m_velocity;
	m_velocity += (m_acc + m_force / m_mass)*dt;

	m_force = 0;
}


}
}
