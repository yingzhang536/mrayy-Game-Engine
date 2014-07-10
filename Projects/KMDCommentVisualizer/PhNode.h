



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
	math::vector2d m_force;

	float m_mass;

	float m_radius;
public:
	PhNode();
	virtual ~PhNode();

	void SetRadius(float r);
	float GetRadius()const;

	void SetMass(float mass);
	float GetMass()const;

	void SetPosition(const math::vector2d& pos);
	const math::vector2d&  GetPosition()const;

	void SetVelocity(const math::vector2d& v);
	const math::vector2d&  GetVelocity()const;

	void SetAcceleration(const math::vector2d& a);
	const math::vector2d&  GetAcceleration()const;

	void AddForce(const math::vector2d& v);

	virtual void Update(float dt);
};

}
}

#endif
