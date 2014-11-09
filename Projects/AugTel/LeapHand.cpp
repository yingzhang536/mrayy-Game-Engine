

#include "stdafx.h"
#include "LeapHand.h"
#include "LeapFinger.h"
#include "LeapHandController.h"
#include "LeapFunctions.h"



namespace mray
{


LeapHand::LeapHand(LeapHandController* c)
{
	m_handController = c;
	m_palm = 0;
	m_foream = 0;
	m_wristJoint = 0;

	for (int i = 0; i < (int)ELeapFinger::Count; ++i)
	{
		m_fingers[i]=new LeapFinger(this,(Leap::Finger::Type)i);
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
void LeapHand::SetHand(Leap::Hand  h)
{
	m_hand = h;
	Leap::FingerList fingers= m_hand.fingers();
	for (int i = 0; i < fingers.count(); ++i)
	{
		m_fingers[i]->SetFinger(fingers[i]);
	}
	/*for (int i = 0; i < (int)ELeapFinger::Count; ++i)
	{
		Leap::Finger f = h.finger(m_fingers[i]->GetType());
		m_fingers[i]->SetFinger(f);
	}*/
}


math::vector3d LeapHand::GetElbowPosition()
{
	math::vector3d pos = LeapToVector3d(m_hand.arm().elbowPosition());
	if (m_handController->GetTransform())
		pos = m_handController->GetTransform()->getAbsoluteTransformation()*pos;
	return pos;
}



math::vector3d LeapHand::GetArmDirection()
{
	math::vector3d dir = LeapToVector3d(m_hand.arm().direction());

	if (m_handController->GetTransform())
		dir = m_handController->GetTransform()->getAbsoluteOrintation()*dir;
	return dir;

}

math::quaternion LeapHand::GetArmRotation()
{
	math::matrix4x4 m;
	math::quaternion q;
	LeapToMatrix(m_hand.arm().basis(), m);
	q.fromMatrix(m);

	if (m_handController->GetTransform())
		q = m_handController->GetTransform()->getAbsoluteOrintation()*q;
	return q;
}


math::vector3d LeapHand::GetWristPosition()
{
	math::vector3d pos = LeapToVector3d(m_hand.arm().wristPosition());
	if (m_handController->GetTransform())
		pos = m_handController->GetTransform()->getAbsoluteTransformation()*pos;
	return pos;
}


math::vector3d LeapHand::GetPalmPosition()
{
	math::vector3d pos = LeapToVector3d(m_hand.palmPosition());
	if (m_handController->GetTransform())
		pos = m_handController->GetTransform()->getAbsoluteTransformation()*pos;
	pos+=GetPalmOffset();
	return pos;
}

math::vector3d LeapHand::GetPalmOffset()
{
	math::vector3d additionalMovement = m_handController->GetHandMovementScale()-math::vector3d(1);
	math::vector3d scaledPosition = LeapToVector3d(m_hand.palmPosition())*additionalMovement;
	if (m_handController->GetTransform())
		scaledPosition=m_handController->GetTransform()->getAbsoluteTransformation()*scaledPosition - m_handController->GetTransform()->getAbsolutePosition();

	return scaledPosition;
}

math::quaternion LeapHand::GetPalmRotation()
{
	math::matrix4x4 m;
	math::quaternion q;
	LeapToMatrix(m_hand.basis(), m);
	q.fromMatrix(m);

	if (m_handController->GetTransform())
		q=m_handController->GetTransform()->getAbsoluteOrintation()*q;
	return q;
}

math::vector3d LeapHand::GetPalmDirection()
{
	math::vector3d dir= LeapToVector3d(m_hand.direction());

	if (m_handController->GetTransform())
		dir = m_handController->GetTransform()->getAbsoluteOrintation()*dir;
	return dir;
}

math::vector3d LeapHand::GetPalmNormal()
{
	math::vector3d dir = LeapToVector3d(m_hand.palmNormal());

	if (m_handController->GetTransform())
		dir = m_handController->GetTransform()->getAbsoluteOrintation()*dir;
	return dir;

}

 void LeapHand::UpdatePosition()
{
	 if (!m_hand.isValid())
		 return;
	 if (m_palm)
	 {
		 m_palm->setPosition(GetPalmPosition());
		 m_palm->setOrintation(GetPalmRotation());
	 }
	 if (m_foream)
	 {
		 m_foream->setPosition(GetElbowPosition());
		 m_foream->setOrintation(GetArmRotation());
	 }
	 if (m_wristJoint)
	 {
		 m_wristJoint->setPosition(GetWristPosition());
		 m_wristJoint->setOrintation(GetPalmRotation());
	 }
	 for (int i = 0; i < (int)ELeapFinger::Count; ++i)
		 m_fingers[i]->UpdatePosition();
}

 void LeapHand::DrawDebug()
 {
	 if (!m_hand.isValid())
		 return;

	 video::IVideoDevice* dev= gEngine.getDevice();

	 math::vector3d elbow = GetElbowPosition();
	 math::vector3d wrist = GetWristPosition();
	 math::vector3d palm = GetPalmPosition();
	 dev->set3DMode();
	 dev->setTransformationState(video::TS_WORLD, math::matrix4x4::Identity);
	 dev->unuseShader();
	 dev->draw3DLine(elbow, wrist, video::SColor(1, 0, 0, m_hand.confidence()));
	 dev->draw3DLine(wrist, palm, video::SColor(0, 1, 0, m_hand.confidence()));

	 for (int i = 0; i < (int)ELeapFinger::Count; ++i)
	 {
		 dev->draw3DLine(palm, m_fingers[i]->GetJointPosition(0), video::SColor(1, 1, 0, m_hand.confidence()));
		 m_fingers[i]->DrawDebug();

	 }
 }

}