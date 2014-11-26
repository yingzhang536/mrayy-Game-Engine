


#ifndef LeapHand_h__
#define LeapHand_h__

#include "leap/Leap.h"


namespace mray
{

	class LeapFinger;
	class LeapHandController;

	enum class ELeapFinger
	{
		Thumb,
		Index,
		Middle,
		Ring,
		Little,

		Count
	};
	
class LeapHand
{
protected:
	Leap::Controller *m_LeapController;
	Leap::Hand m_hand;

	LeapHandController* m_handController;

	LeapFinger* m_fingers[(int)ELeapFinger::Count];
	scene::IMovable* m_palm;
	scene::IMovable* m_foream;
	scene::IMovable* m_wristJoint;
public:
	LeapHand(LeapHandController* c);
	virtual ~LeapHand();


	void SetPalmNode(scene::IMovable* p){
		m_palm = p;
	}
	void SetForeamNode(scene::IMovable*f){
		m_foream = f;
	}
	void SetWristJoint(scene::IMovable* w){
		m_wristJoint = w;
	}

	LeapFinger* GetFinger(ELeapFinger finger);
	LeapHandController* Controller(){ return m_handController; }

	void SetHand(Leap::Hand  h);
	Leap::Hand GetHand(){ return m_hand; }

	math::vector3d GetElbowPosition();

	math::vector3d GetArmDirection();
	math::quaternion GetArmRotation();

	math::vector3d GetWristPosition();

	math::vector3d GetPalmPosition();
	math::vector3d GetPalmOffset();
	math::quaternion GetPalmRotation();
	math::vector3d GetPalmDirection();
	math::vector3d GetPalmNormal();

	virtual void UpdatePosition();

	virtual void DrawDebug(scene::IDebugDrawManager* dbg);
};

}

#endif // LeapHand_h__
