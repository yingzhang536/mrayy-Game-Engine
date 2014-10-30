


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
	Leap::Hand *m_hand;

	LeapHandController* m_handController;

	LeapFinger* m_fingers[(int)ELeapFinger::Count];
public:
	LeapHand();
	virtual ~LeapHand();

	LeapFinger* GetFinger(ELeapFinger finger);

	Leap::Hand *GetHand(){ return m_hand; }

	math::vector3d GetElbowPosition();

	math::vector3d GetArmCenter();
	math::vector3d GetArmDirection();
	math::quaternion GetArmRotation();

	math::vector3d GetWristPosition();

	math::vector3d GetPalmPosition();
	math::vector3d GetPalmOffset();
	math::quaternion GetPalmRotation();
	math::vector3d GetPalmDirection();
	math::vector3d GetPalmNormal();

};

}

#endif // LeapHand_h__
