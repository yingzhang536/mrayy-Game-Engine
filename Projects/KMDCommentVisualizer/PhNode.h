



/********************************************************************
	created:	2014/05/07
	created:	7:5:2014   15:40
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\PhNode.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	PhNode
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __PhNode__
#define __PhNode__




namespace mray
{
namespace physics
{

class PhNode
{
protected:
	math::vector2d m_position;
	math::vector2d m_velocity;
	math::vector2d m_acc;
	math::vector2d m_force;

	float m_mass;

	float m_radius;
	float m_drag;

	bool m_fixed;
public:
	PhNode();
	virtual ~PhNode();

	void SetFixed(bool f){ m_fixed = f; }
	bool IsFixed(){ return m_fixed; }

	void SetRadius(float r){ m_radius = r; }
	float GetRadius()const{ return m_radius; }

	void SetMass(float mass){ m_mass = mass; }
	float GetMass()const{ return m_mass; }

	void SetPosition(const math::vector2d& pos){ m_position = pos; }
	const math::vector2d&  GetPosition()const{ return m_position; }

	void SetVelocity(const math::vector2d& v){ m_velocity = v; }
	const math::vector2d&  GetVelocity()const{ return m_fixed ? math::vector2d::Zero : m_velocity; }

	void SetAcceleration(const math::vector2d& a){ m_acc = a; }
	const math::vector2d&  GetAcceleration()const{ return m_fixed ? math::vector2d::Zero : m_acc; }

	void AddForce(const math::vector2d& v);

	virtual void Update(float dt);
};

}
}

#endif
