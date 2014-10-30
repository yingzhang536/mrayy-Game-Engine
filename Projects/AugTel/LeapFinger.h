

#ifndef LeapFinger_h__
#define LeapFinger_h__

#include "leap/Leap.h"

namespace mray
{
	
class LeapFinger
{
protected:
	Leap::Finger m_finger;
	LeapHandController* m_hand;
public:
	LeapFinger(LeapHandController* hand);
	virtual ~LeapFinger();

	LeapHandController* GetHand(){
		return m_hand;
	}

	void SetOffset(const math::vector3d &offset);
	const math::vector3d& GetOffset();
	
	math::vector3d GetTipPosition();
	math::vector3d GetPointingDirection();
	math::vector3d GetJointPosition(Leap::Bone::Type boneType);
	math::vector3d GetBoneCenter(Leap::Bone::Type boneType);
	math::vector3d GetBoneDirection(Leap::Bone::Type boneType);
	math::quaternion GetBoneRotation(Leap::Bone::Type boneType);


};

}

#endif // LeapFinger_h__
