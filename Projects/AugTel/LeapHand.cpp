

#include "stdafx.h"
#include "LeapHand.h"
#include "LeapFinger.h"
#include "LeapHandController.h"
#include "LeapFunctions.h"



namespace mray
{


LeapHand::LeapHand()
{
	for (int i = 0; i < (int)ELeapFinger::Count; ++i)
	{
		m_fingers[i]=new LeapFinger(this);
	}
}
LeapHand::~LeapHand()
{
	for (int i = 0; i < (int)ELeapFinger::Count;++i)
	{
		delete m_fingers[i];
	}
}

LeapFinger* LeapHand::GetFinger(ELeapFinger finger)
{
	return m_fingers[(int)finger];
}


Leap::Hand LeapHand::GetHand()
{
	return m_hand; 
}

math::vector3d LeapHand::GetElbowPosition()
{

}


math::vector3d LeapHand::GetArmCenter()
{

}

math::vector3d LeapHand::GetArmDirection()
{

}

math::quaternion LeapHand::GetArmRotation()
{

}


math::vector3d LeapHand::GetWristPosition()
{

}


math::vector3d LeapHand::GetPalmPosition()
{

}

math::vector3d LeapHand::GetPalmOffset()
{
	if (!m_hand)
		return math::vector3d::Zero;
	math::vector3d additionalMovement = m_handController->GetHandMovementScale()-math::vector3d(1);
	math::vector3d scaledPosition = LeapToVector3d(m_hand->palmPosition())*additionalMovement;
	if (m_handController->GetTransform())
		scaledPosition=m_handController->GetTransform()->getAbsoluteTransformation()*scaledPosition - m_handController->GetTransform()->getAbsolutePosition();

	return scaledPosition;
}

math::quaternion LeapHand::GetPalmRotation()
{

}

math::vector3d LeapHand::GetPalmDirection()
{

}

math::vector3d LeapHand::GetPalmNormal()
{

}

}