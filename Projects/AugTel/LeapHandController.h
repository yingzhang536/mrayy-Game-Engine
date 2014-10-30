
#ifndef LeapHandController_h__
#define LeapHandController_h__



namespace mray
{
	namespace scene
	{
		class IMovable;
	}
	class LeapHand;
class LeapHandController
{
protected:
	math::vector3d m_handMovementScale;
	scene::IMovable* m_transform;

	LeapHand* m_leftHand;
	LeapHand* m_rightHand;
public:
	LeapHandController();
	virtual ~LeapHandController();

	void SetTransform(scene::IMovable* t){ m_transform=t; }
	scene::IMovable* GetTransform(){ return m_transform; }

	math::vector3d GetHandMovementScale(){ return m_handMovementScale; }

	LeapHand* GetleftHand(){ return m_leftHand; }
	LeapHand* GetRightHand(){ return m_rightHand; }
};

}

#endif // LeapHandController_h__

