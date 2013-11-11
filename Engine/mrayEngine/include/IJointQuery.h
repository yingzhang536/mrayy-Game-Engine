

/********************************************************************
	created:	2010/02/11
	created:	11:2:2010   11:27
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\IJointQuery.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	IJointQuery
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	this is used to make DOF queries on joints,
				this is primary used for InverseKinmatics
*********************************************************************/

#ifndef ___IJointQuery___
#define ___IJointQuery___

#include "compileConfig.h"



namespace mray{
namespace scene{
	class IMovable;
}
namespace animation{

class MRAY_DLL IJointQuery
{
private:
protected:
public:
	struct JointDOF
	{
		bool isRotation;
		scene::IMovable* node;
	};

	IJointQuery(){}
	virtual~IJointQuery(){}

	virtual void getJointsDOF(std::vector<JointDOF>& dof,const math::vector3d&targetPos)=0;
	virtual int getJointsDOFCount()=0;
	virtual math::vector3d getEndEffectorPos()=0;
	virtual math::quaternion getEndEffectorOri()=0;

	virtual void applyChange(const std::vector<math::quaternion> &change)=0;
};

}
}


#endif //___IJointQuery___
