

#include "stdafx.h"
#include "IPhysicsComponent.h"
#include "SceneComponent.h"
#include "ISceneNode.h"
#include "Skeleton.h"
#include "SMesh.h"
#include "BoneNode.h"



namespace mray
{
namespace game
{

PhysicalComponentAttachment::PhysicalComponentAttachment()
{
	m_targetObject=0;
	m_targetNode=0;
	m_inheritPosition=true;
	m_inheritRotation=true;
}
bool PhysicalComponentAttachment::Init(IPhysicsComponent*o)
{
	m_targetObject=0;
	m_targetNode=0;

	if(o->GetOwnerComponent())
	{
		m_targetNode=dynamic_cast<SceneComponent*>(IGameComponent::RetriveComponent(o->GetOwnerComponent(),SceneComponent::getClassType(),m_targetName));

		if(m_targetNode)
		{
			//make sure its inited
			m_targetNode->InitComponent();

			if(m_targetBone!=mT(""))
			{
				scene::ISceneNode* node= m_targetNode->GetSceneNode();
				if(node)
				{
					scene::IRenderable* rnode=  node->GetAttachedNode(0);
					if(rnode)
					{
						scene::Skeleton* sk= rnode->getMesh()->getSkeleton();
						if(sk)
						{
							m_targetObject= sk->getBone(m_targetBone);
						}
					}
				}
			}else
				m_targetObject=m_targetNode->GetSceneNode();
		}
	}
	return true;
}

void PhysicalComponentAttachment::Update(const math::vector3d& pos,const math::quaternion& ori)
{
	if( m_targetObject)
	{
		//update attached movable object
		if(m_inheritPosition)
			m_targetObject->setPosition(pos);

		if(m_inheritRotation)
			m_targetObject->setOrintation(ori);
	}
}

void PhysicalComponentAttachment::LoadFromXML(xml::XMLElement*e)
{
	xml::XMLAttribute* attr;
	attr=e->getAttribute(mT("TargetNode"));
	if(attr)
		m_targetName=attr->value;
	attr=e->getAttribute(mT("TargetBone"));
	if(attr)
		m_targetBone=attr->value;


	attr=e->getAttribute(mT("InheritPosition"));
	if(attr)
		m_inheritPosition=core::StringConverter::toBool(attr->value);
	attr=e->getAttribute(mT("InheritRotation"));
	if(attr)
		m_inheritRotation=core::StringConverter::toBool(attr->value);
}

void PhysicalComponentAttachment::SaveToXML(xml::XMLElement*e)
{
	xml::XMLElement* ee=new xml::XMLElement(mT("Attachment"));
	e->addSubElement(ee);
	ee->addAttribute(mT("TargetNode"),m_targetName);
	ee->addAttribute(mT("TargetBone"),m_targetBone);
	ee->addAttribute(mT("InheritPosition"),core::StringConverter::toString(m_inheritPosition));
	ee->addAttribute(mT("InheritRotation"),core::StringConverter::toString(m_inheritRotation));
}


bool IPhysicsComponent::InitComponent()
{
	if(!IGameComponent::InitComponent())
		return false;
	std::list<PhysicalComponentAttachment*>::iterator it= m_attachments.begin();
	for(;it!=m_attachments.end();++it)
	{
		(*it)->Init(this);
	}
	return true;
}


xml::XMLElement*  IPhysicsComponent::loadXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret=IGameComponent::loadXMLSettings(elem);
	if(!ret)
		return ret;
	xml::XMLElement* att=ret->getSubElement(mT("Attachments"));
	if(att)
	{
		att=att->getSubElement(mT("Attachment"));
		while(att)
		{
			PhysicalComponentAttachment* attComp=new PhysicalComponentAttachment();
			attComp->LoadFromXML(att);
			AttachNode(attComp);
			att=att->nextSiblingElement(mT("Attachment"));
		}
	}
	/*
	xml::XMLElement* e=ret->getSubElement(mT("PhysicalNodeDesc"));
	if(e)
	{
		m_nodeDesc.LoadFromXML(e);
		physics::PhysicXMLSerializer::Parse(e,&m_bodyDesc);
		e=e->getSubElement(mT("PhysicalBodyDesc"));
		if(e)
		{
			m_bodyDesc.LoadFromXML(e);
			m_nodeDesc.bodyDesc=&m_bodyDesc;
		}
		else
			m_nodeDesc.bodyDesc=0;
	}*/
	return ret;
}

xml::XMLElement*  IPhysicsComponent::exportXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret=IGameComponent::exportXMLSettings(elem);
	
	xml::XMLElement*  att=new xml::XMLElement(mT("Attachments"));
	ret->addSubElement(att);

	std::list<PhysicalComponentAttachment*>::iterator it= m_attachments.begin();
	for(;it!=m_attachments.end();++it)
	{
		(*it)->SaveToXML(att);
	}
	return ret;
}


}
}
