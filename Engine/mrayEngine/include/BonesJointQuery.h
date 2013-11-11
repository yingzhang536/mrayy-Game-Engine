
/********************************************************************
	created:	2010/02/11
	created:	11:2:2010   12:18
	file base:	BonesJointQuery
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___BonesJointQuery___
#define ___BonesJointQuery___

#include "IJointQuery.h"

namespace mray{

	namespace scene
	{
		class BoneNode;
	}

namespace animation{

class MRAY_DLL BonesJointQuery:public IJointQuery
{
private:
protected:
	scene::BoneNode* m_endEffector;
	std::vector<scene::BoneNode*> m_bones;
	int m_dofs;
public:
	BonesJointQuery();
	virtual~BonesJointQuery();

	void addBone(scene::BoneNode*bone,bool endEffector=false);
	void clearBones();

	virtual void getJointsDOF(std::vector<JointDOF>& dof,const math::vector3d&target);
	virtual int getJointsDOFCount();

	virtual math::vector3d getEndEffectorPos();
	virtual math::quaternion getEndEffectorOri();

	virtual void applyChange(const std::vector<math::quaternion> &change);
};

}
}


#endif //___BonesJointQuery___
