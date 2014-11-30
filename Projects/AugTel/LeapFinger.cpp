

#include "stdafx.h"
#include "LeapFinger.h"
#include "LeapFunctions.h"
#include "LeapHand.h"
#include "LeapHandController.h"
#include "IDebugDrawManager.h"

namespace mray
{


LeapFinger::LeapFinger(LeapHand* hand, Leap::Finger::Type type)
{
	m_hand = hand;
	m_type = type;
	for (int i = 0; i < BoneCount; ++i)
		m_nodes[i] = 0;

	math::vector3d fingerPointing = math::vector3d::XAxis;
	math::vector3d palmFacing = -math::vector3d::YAxis;

	math::quaternion q = QuatLookRotation(fingerPointing, -palmFacing);
 //	m_reorient = q.inverse();

}

LeapFinger::~LeapFinger()
{
}



void LeapFinger::SetOffset(const math::vector3d &offset)
{
	m_offset = offset;
}

const math::vector3d& LeapFinger::GetOffset()
{
	return m_offset;
}


math::vector3d LeapFinger::GetTipPosition()
{
	math::vector3d tip = LeapToVector3d(m_finger.bone(Leap::Bone::TYPE_DISTAL).nextJoint()) + m_offset;
	if (m_hand->Controller()->GetTransform())
		tip=m_hand->Controller()->GetTransform()->getAbsoluteTransformation()*tip;
	return tip;
}

math::Ray3d LeapFinger::GetPointingRay()
{
	math::Ray3d r;
	r.Start = GetTipPosition();
	r.Dir = GetBoneDirection(BoneCount-1);
	return r;
}

math::vector3d LeapFinger::GetJointPosition(int boneType)
{
	if (boneType >= BoneCount)
		return GetTipPosition();
	math::vector3d tip = LeapToVector3d(m_finger.bone((Leap::Bone::Type)boneType).prevJoint()) + m_offset;
	if (m_hand->Controller()->GetTransform())
		tip = m_hand->Controller()->GetTransform()->getAbsoluteTransformation()*tip;
	return tip;
}

math::vector3d LeapFinger::GetBoneCenter(int boneType)
{
	math::vector3d p = LeapToVector3d(m_finger.bone((Leap::Bone::Type)boneType).center()) + m_offset;
	if (m_hand->Controller()->GetTransform())
		p = m_hand->Controller()->GetTransform()->getAbsoluteTransformation()*p;
	return p;
}

math::vector3d LeapFinger::GetBoneDirection(int boneType)
{
	math::vector3d dir = GetJointPosition(boneType + 1) - GetJointPosition(boneType);
	dir.Normalize();
	return dir;
}

math::quaternion LeapFinger::GetBoneRotation(int boneType)
{
	math::quaternion q=LeapToQuaternion(m_finger.bone((Leap::Bone::Type)boneType).basis());


	if (m_hand->Controller()->GetTransform())
		q = m_hand->Controller()->GetTransform()->getAbsoluteOrintation()*q;
	return q;
}

void LeapFinger::UpdatePosition()
{
	if (!m_finger.isValid())
		return;
	for (int i = 0; i < BoneCount; ++i)
	{
		if (m_nodes[i])
		{
			m_nodes[i]->setPosition(GetJointPosition(i));
			math::quaternion parentInv;
			if (m_nodes[i]->getParent())
				parentInv = m_nodes[i]->getParent()->getAbsoluteOrintation().inverse();
			m_nodes[i]->setOrintation(GetBoneRotation(i)*m_reorient);// m_baseMat[i] * m_reorient);// GetBoneRotation(i)*m_reorient);
		}
	}
}

void LeapFinger::DrawDebug(scene::IDebugDrawManager* dbg)
{
	if (!m_finger.isValid())
		return;
	for (int i = 0; i < BoneCount-1; ++i)
	{
		math::vector3d p1, p2;
		p1 = GetJointPosition(i);
		p2 = GetJointPosition(i+1);
		dbg->AddLine(p1, p2, video::SColor(1, 1, 1, 1), 1, 0);
	}
}

}

