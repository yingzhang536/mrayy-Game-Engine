

#include "stdafx.h"
#include "KinBodyControlComponent.h"
#include "PhysicsComponent.h"


namespace mray
{
namespace VT
{


	IMPLEMENT_PROPERTY_TYPE_HEADER(TargetNode,KinBodyControlComponent,core::string,mT("TargetNode"),EPBT_Basic,mT(""),mT(""));
	IMPLEMENT_PROPERTY_TYPE_GENERIC(TargetNode,KinBodyControlComponent,core::string,SetTargetName,GetTargetName,,,false);


KinBodyControlComponent::KinBodyControlComponent(game::GameEntityManager*mngr)
{
	m_attachedBody=0;
	CPropertieDictionary* dic;
	if(CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeTargetNode::instance);
	}
}

KinBodyControlComponent::~KinBodyControlComponent()
{
}


bool KinBodyControlComponent::InitComponent()
{
	if(!IVTComponent::InitComponent())
		return false;
	m_attachedBody=RetriveComponent<game::PhysicsComponent>(GetOwnerComponent(),m_targetBody);
	if(!m_attachedBody)
	{
		gLogManager.log("KinBodyControlComponent::InitComponent() - Warning Body named:\""+m_targetBody+"\" couldn't be found in \""+GetName()+"\"",ELL_WARNING);
		return true;
	}
	m_attachedBody->InitComponent();
	m_initalPos=m_attachedBody->GetPosition();
	m_initialRot=m_attachedBody->GetRotation();

	if(m_attachedBody->GetPhysicalNode())
		m_attachedBody->GetPhysicalNode()->SetBodyFlag(physics::EBF_Kinematic,true);
	return true;
}


bool KinBodyControlComponent::SetEnabled(bool enabled)
{
	return true;
}


bool KinBodyControlComponent::SetControlValue(const std::vector<ControlInputValues>&  v,EControlSource src)
{
	if(v.size()<0)
		return false;
	IControllableComponent::SetControlValue(v,src);
	m_targetValue.resize(v.size());
	for (int i=0;i<v.size();++i)
	{
		m_targetValue[i]=v[i];
	}

	math::vector3d pos;
	math::quaternion rot;
	
	if(m_targetValue.size()>=3)
	{
		pos.set(m_targetValue[0].KinValue,m_targetValue[1].KinValue,m_targetValue[2].KinValue);
		pos+=m_initalPos;
		m_attachedBody->GetPhysicalNode()->moveGlobalPosition(pos);
	}
	if(m_targetValue.size()>=7)
	{
		rot=math::quaternion(m_targetValue[3].KinValue,m_targetValue[4].KinValue,m_targetValue[5].KinValue,m_targetValue[6].KinValue);
		rot.Normalize();
		rot*=m_initialRot;
		if(m_attachedBody->GetPhysicalNode())
			m_attachedBody->GetPhysicalNode()->moveGlobalOrientation(rot);
	}



	return true;
}


void KinBodyControlComponent::Update(float dt)
{
}


std::vector<float> KinBodyControlComponent::GetCurrentValue()
{
	math::vector3d pos=m_attachedBody->GetPhysicalNode()->getGlobalPosition();
	math::quaternion ori=m_attachedBody->GetPhysicalNode()->getGlobalOrintation();
	std::vector<float> vec;
	vec.push_back(pos.x);
	vec.push_back(pos.y);
	vec.push_back(pos.z);
	vec.push_back(ori.x);
	vec.push_back(ori.y);
	vec.push_back(ori.z);
	vec.push_back(ori.w);

	return vec;
}

}
}
