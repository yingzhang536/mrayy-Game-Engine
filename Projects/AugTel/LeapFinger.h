

#ifndef LeapFinger_h__
#define LeapFinger_h__

#include "leap/Leap.h"

namespace mray
{
	class LeapHand;
	
class LeapFinger
{
public: 
	static const int BoneCount = 4;
	static const int JointCount = 5;
protected:
	Leap::Finger m_finger;
	LeapHand* m_hand;
	math::vector3d m_offset;
	Leap::Finger::Type m_type;
	scene::IMovable* m_nodes[BoneCount];

	math::quaternion m_baseMat[BoneCount];

	math::quaternion m_reorient;
public:
	LeapFinger(LeapHand* hand, Leap::Finger::Type type);
	virtual ~LeapFinger();

	void SetFinger(Leap::Finger f)
	{
		m_finger = f; 
		bool valid= f.isValid();
		valid = true;
	}
	Leap::Finger GetFinger(){ return m_finger; }
	void SetNode(scene::IMovable* n, int boneType){ 
		m_nodes[boneType] = n;
		if (n)
		{
			m_baseMat[boneType] = n->getOrintation();
		}
	}
	scene::IMovable* GetNode(int boneType){ return  m_nodes[boneType]; }

	LeapHand* GetHand(){
		return m_hand;
	}
	Leap::Finger::Type GetType(){ return m_type; }

	void SetOffset(const math::vector3d &offset);
	const math::vector3d& GetOffset();
	
	math::vector3d GetTipPosition();
	math::Ray3d GetPointingRay();
	math::vector3d GetJointPosition(int boneType);
	math::vector3d GetBoneCenter(int boneType);
	math::vector3d GetBoneDirection(int boneType);
	math::quaternion GetBoneRotation(int boneType);

	void UpdatePosition();

	void DrawDebug(scene::IDebugDrawManager* dbg);
};

}

#endif // LeapFinger_h__
