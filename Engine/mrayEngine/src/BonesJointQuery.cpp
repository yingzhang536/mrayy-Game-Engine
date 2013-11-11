#include "stdafx.h"
 
#include "BonesJointQuery.h"
#include "BoneNode.h"


namespace mray{
namespace animation{

BonesJointQuery::BonesJointQuery()
{
	m_dofs=0;
	m_endEffector=0;
}
BonesJointQuery::~BonesJointQuery()
{
}

void BonesJointQuery::addBone(scene::BoneNode*bone,bool endEffector)
{
	if(!bone)return;

	if(endEffector)
		m_endEffector=bone;
	//else
	{
		m_bones.push_back(bone);
		//m_dofs+=3;
		m_dofs++;
	}
}

void BonesJointQuery::clearBones()
{
	m_bones.clear();
	m_endEffector=0;
	m_dofs=0;
}

void BonesJointQuery::getJointsDOF(std::vector<JointDOF>& dof,const math::vector3d&target)
{
	dof.resize(m_dofs);
	math::vector3d ef=getEndEffectorPos();
	
	for(int i=0;i<m_bones.size();++i)
	{
		scene::BoneNode* bone=m_bones[i];
		//tmp.pivot=bone->getAbsolutePosition();
		dof[i].node=bone;
		dof[i].isRotation=true;
	}
	/*
	for(int i=0,j=0;j<m_bones.size();i+=3,++j)
	{
		scene::BoneNode* bone=m_bones[j];
		tmp.pivot=bone->getAbsolutePosition();
		const math::matrix4x4&m=bone->getAbsoluteTransformation();
		//3 rotation Axis
		tmp.axis.x=m(0,0);
		tmp.axis.y=m(1,0);
		tmp.axis.z=m(2,0);
		dof[i]=tmp;
		tmp.axis.x=m(0,1);
		tmp.axis.y=m(1,1);
		tmp.axis.z=m(2,1);
		dof[i+1]=(tmp);
		tmp.axis.x=m(0,2);
		tmp.axis.y=m(1,2);
		tmp.axis.z=m(2,2);
		dof[i+2]=(tmp);
	}*/
}
int BonesJointQuery::getJointsDOFCount()
{
	return m_dofs;
}

math::vector3d BonesJointQuery::getEndEffectorPos()
{
	FATAL_ERROR(!m_endEffector,mT("End-Effector not set"));
	return m_endEffector->getAbsolutePosition();
}
math::quaternion BonesJointQuery::getEndEffectorOri()
{
	FATAL_ERROR(!m_endEffector,mT("End-Effector not set"));
	return m_endEffector->getAbsoluteOrintation();
}

void BonesJointQuery::applyChange(const std::vector<math::quaternion> &change)
{
	for(int i=0;i<m_bones.size();++i)
	{
		scene::BoneNode* bone=m_bones[i];
		bone->rotate(change[i],scene::TS_Parent);

		bone->updateAbsoluteTransformation();
	}

	m_endEffector->updateAbsoluteTransformation();
	/*
	for(int i=0,j=0;j<m_bones.size();i+=3,++j)
	{
		scene::BoneNode* bone=m_bones[j];
		bone->rotate(math::quaternion(change[i+0],dof[i+0].axis),scene::TS_World);
		bone->rotate(math::quaternion(change[i+1],dof[i+1].axis),scene::TS_World);
		bone->rotate(math::quaternion(change[i+2],dof[i+2].axis),scene::TS_World);

		bone->updateAbsoluteTransformation();
	}*/
}

}
}