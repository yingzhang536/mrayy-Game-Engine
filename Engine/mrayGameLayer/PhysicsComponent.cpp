
#include "stdafx.h"
#include "PhysicsComponent.h"
#include "SceneComponent.h"
#include "GameEntity.h"
#include "IMovable.h"
#include "IPhysicManager.h"
#include "ISceneNode.h"
#include "ICollisionComponent.h"
#include "GameEntityManager.h"
#include "PhysicXMLSerializer.h"
#include "XMLTree.h"
#include "IFileSystem.h"



namespace mray
{
namespace game
{

// 	IMPLEMENT_PROPERTY_TYPE_HEADER(InheritPosition,PhysicsComponent,bool,mT("InheritPosition"),EPBT_Basic,mT(""),true);
// 	IMPLEMENT_PROPERTY_TYPE_GENERIC(InheritPosition,PhysicsComponent,bool,SetInheritPosition,GetInheritPosition,core::StringConverter::toString,core::StringConverter::toBool,false);
// 
// 	IMPLEMENT_PROPERTY_TYPE_HEADER(InheritRotation,PhysicsComponent,bool,mT("InheritRotation"),EPBT_Basic,mT(""),true);
// 	IMPLEMENT_PROPERTY_TYPE_GENERIC(InheritRotation,PhysicsComponent,bool,SetInheritRotation,GetInheritRotation,core::StringConverter::toString,core::StringConverter::toBool,false);
// 
// 	IMPLEMENT_PROPERTY_TYPE_HEADER(TargetNode,PhysicsComponent,core::string,mT("TargetNode"),EPBT_Basic,mT("Target Scene Node Name"),mT(""));
// 	IMPLEMENT_PROPERTY_TYPE_GENERIC(TargetNode,PhysicsComponent,core::string,SetTargetName,GetTargetName,,,false);
// 
// 	IMPLEMENT_PROPERTY_TYPE_HEADER(TargetBone,PhysicsComponent,core::string,mT("TargetBone"),EPBT_Basic,mT("Target Bone Name"),mT(""));
// 	IMPLEMENT_PROPERTY_TYPE_GENERIC(TargetBone,PhysicsComponent,core::string,SetTargetBone,GetTargetBone,,,false);

IMPLEMENT_PROPERTY_TYPE_HEADER(IsDynamic,PhysicsComponent,bool,mT("IsDynamic"),EPBT_Basic,mT(""),true);
IMPLEMENT_PROPERTY_TYPE_GENERIC(IsDynamic,PhysicsComponent,bool,SetIsDynamic,GetIsDynamic,core::StringConverter::toString,core::StringConverter::toBool,false);

IMPLEMENT_PROPERTY_TYPE_HEADER(PhysicsModel,PhysicsComponent,core::string,mT("PhysicsModel"),EPBT_Basic,mT(""),mT(""));
IMPLEMENT_PROPERTY_TYPE_GENERIC(PhysicsModel,PhysicsComponent,core::string,SetPhysicsModel,GetPhysicsModel,,,false);

IMPLEMENT_PROPERTY_TYPE_HEADER(Position,PhysicsComponent,math::vector3d,mT("Position"),EPBT_Basic,mT(""),0);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Position,PhysicsComponent,math::vector3d,SetPosition,GetPosition,core::StringConverter::toString,core::StringConverter::toVector3d,false);

IMPLEMENT_PROPERTY_TYPE_HEADER(Rotation,PhysicsComponent,math::vector3d,mT("Rotation"),EPBT_Basic,mT(""),0);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Rotation,PhysicsComponent,math::vector3d,SetRotation,GetRotation,core::StringConverter::toString,core::StringConverter::toVector3d,false);

IMPLEMENT_PROPERTY_TYPE_HEADER(Density,PhysicsComponent,float,mT("Density"),EPBT_Basic,mT(""),1);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Density,PhysicsComponent,float,SetDensity,GetDensity,core::StringConverter::toString,core::StringConverter::toFloat,false);

IMPLEMENT_PROPERTY_TYPE_HEADER(ActorGroupID,PhysicsComponent,ushort,mT("ActorGroupID"),EPBT_Basic,mT(""),0);
IMPLEMENT_PROPERTY_TYPE_GENERIC(ActorGroupID,PhysicsComponent,ushort,SetActorGroupID,GetActorGroupID,core::StringConverter::toString,core::StringConverter::toUInt,false);

PhysicsComponent::PhysicsComponent(GameEntityManager*mngr):m_node(0),m_removeNodeOnDestroy(0)
{
	m_isDynamic=true;
	m_phManager=mngr->GetPhysicsManager();
	_addProperties();
}
PhysicsComponent::PhysicsComponent(physics::IPhysicManager* phManager):m_node(0),m_removeNodeOnDestroy(0)
{
	m_isDynamic=true;
	m_phManager=phManager;
	_addProperties();
	InitComponent();
}
void PhysicsComponent::_addProperties()
{
	PropertyTypeIsDynamic::instance.setFlag(EPF_Serialize,false);
	PropertyTypeDensity::instance.setFlag(EPF_Serialize,false);
	PropertyTypeActorGroupID::instance.setFlag(EPF_Serialize,false);
	CPropertieDictionary* dic;
	if(CreateDictionary(&dic))
	{
// 		dic->addPropertie(&PropertyTypeTargetNode::instance);
// 		dic->addPropertie(&PropertyTypeTargetBone::instance);
// 		dic->addPropertie(&PropertyTypeInheritPosition::instance);
// 		dic->addPropertie(&PropertyTypeInheritRotation::instance);
		dic->addPropertie(&PropertyTypeIsDynamic::instance);
		dic->addPropertie(&PropertyTypePosition::instance);
		dic->addPropertie(&PropertyTypeRotation::instance);
		dic->addPropertie(&PropertyTypeDensity::instance);
		dic->addPropertie(&PropertyTypeActorGroupID::instance);
		dic->addPropertie(&PropertyTypePhysicsModel::instance);
	}
}

PhysicsComponent::~PhysicsComponent()
{
	if(m_removeNodeOnDestroy)
	{
		m_node->GetCreator()->removeNode(m_node);
		//delete m_node;
	}
	std::list<PhysicalComponentAttachment*>::iterator it= m_attachments.begin();
	for (;it!=m_attachments.end();++it)
	{
		delete *it;
	}
}

bool PhysicsComponent::SetName(const core::string& name)
{
	IGameComponent::SetName(name);
	m_nodeDesc.name=name;
	_Rebuild();
	return true;
}
bool PhysicsComponent::SetPhysicsModel(const core::string& path)
{
	m_physicsModel=path;
	_Rebuild();
	return true;
}


bool PhysicsComponent::AddComponent(IObjectComponent* comp)
{
	if(!IGameComponent::AddComponent(comp))
		return false;
	ICollisionComponent* coll=dynamic_cast<ICollisionComponent*>(comp);
	if(coll )
	{
		m_collShapes.push_back(coll);
		if(m_node)
			m_node->CreateShape(coll->GetShapeDesc());
		else
			_Rebuild();
	}
	return true;
}

void PhysicsComponent::RemoveComponent(IObjectComponent* comp)
{
	IGameComponent::RemoveComponent(comp);
	ICollisionComponent* coll=dynamic_cast<ICollisionComponent*>(comp);
	if(coll )
	{
		std::list<ICollisionComponent*>::iterator it= m_collShapes.begin();
		for(;it!=m_collShapes.end();++it)
		{
			if(*it==comp)
			{
				m_collShapes.erase(it);
		//		_Rebuild();
				break;
			}
		}
	}
}

void PhysicsComponent::_Rebuild()
{
	if(!m_inited)
	{
		if(m_node)
		{
			m_node->SetName(m_nodeDesc.name);
		}
		return;
	}
	if(!m_node)
	{
		if(m_physicsModel!=mT(""))
		{
			xml::XMLTree tree;
			if(tree.load(gFileSystem.openFile(m_physicsModel)))
			{
				m_nodeDesc.LoadFromXML(tree.getSubElement(mT("PhysicalNode")));
				if(m_nodeDesc.bodyDesc)
				{
					m_isDynamic=true;
					m_bodyDesc=*m_nodeDesc.bodyDesc;
					m_bodyDesc.autoDelete=false;
					m_bodyDesc.contactReportThreshold=0.1/1024.f;
				}else
					m_isDynamic=false;
				/*
				for(int i=0;m_nodeDesc.shapes.size();i++)
				{
					ICollisionComponent* comp=0;
					switch(m_nodeDesc.shapes[i]->getType())
					{
					case physics::ESS_Convex:
						{
						ModelCollisionComponent* shape=new ModelCollisionComponent();
						shape->SetModelPath(m_nodeDesc.shapes[i]->
				}*/
			}
		}else
		{
			//m_nodeDesc.shapes.clear();
			std::list<ICollisionComponent*>::iterator it= m_collShapes.begin();
			for(;it!=m_collShapes.end();++it)
			{
				physics::IPhysicalShapeDesc*d= (*it)->GetShapeDesc();
				m_nodeDesc.shapes.push_back(d);
			}
		}

		if(!m_nodeDesc.shapes.size())
		{
			SetPhysicalNode(0);
			return;
		}
		for(int i=0;i<m_nodeDesc.shapes.size();++i)
		{
			m_nodeDesc.shapes[i]->flags|=physics::ESF_ContactForce;
		}
		math::matrix4x4 m;
		math::quaternion(m_rotation).toMatrix(m);
		m.setTranslation(m_position);
		m_nodeDesc.globalPos=m*m_nodeDesc.globalPos;

		if(m_nodeDesc.bodyDesc && m_nodeDesc.bodyDesc->autoDelete)
			delete m_nodeDesc.bodyDesc;
		if(m_isDynamic)
			m_nodeDesc.bodyDesc=&m_bodyDesc;
		else
			m_nodeDesc.bodyDesc=0;
		m_nodeDesc.userData=this;
		SetPhysicalNode(m_phManager->createNode(&m_nodeDesc));
	}else
	{
		//
		//m_node->SaveToDesc(&m_bodyDesc);
	}
}
bool PhysicsComponent::InitComponent()
{
	if(!IPhysicsComponent::InitComponent())
		return false;
	_Rebuild();
	_OnTargetNodeChanged();

	return true;
}
void PhysicsComponent::OnAttachedToComponent(IObjectComponent* owner)
{
	IGameComponent::OnAttachedToComponent(owner);
	_OnTargetNodeChanged();
}

void PhysicsComponent::OnRemovedFromComponent(IObjectComponent* owner)
{
	IGameComponent::OnRemovedFromComponent(owner);
}

bool PhysicsComponent::SetEnabled(bool enabled)
{
	IGameComponent::SetEnabled(enabled);
	if(m_node)
	{
		m_node->SetBodyFlag(physics::EBF_Kinematic,!IsEnabled());
		m_node->SetBodyFlag(physics::EBF_Frozen, !IsEnabled());
	}
	return true;
}
void PhysicsComponent::_OnTargetNodeChanged()
{
}

void PhysicsComponent::SetPhysicalNode(physics::IPhysicalNode* node)
{
	m_node=node;
	//AddSubSet(mT("PhsyicalNode"),m_node);
	if(m_node)
	{
		m_node->EnableContactReportFlag(physics::ECRF_TouchThreshold);
		m_node->SetContactReportThreshold(1.0f/1024.f);
		//m_node->SetBodyFlag(physics::EBF_Kinematic,!IsEnabled());
	}
}

bool PhysicsComponent::SetPosition(const math::vector3d& v)
{
	m_position=v;
	if(m_node)
	{
		m_node->setGlobalPosition(v);
	}
	return true;
}
const math::vector3d& PhysicsComponent::GetPosition()
{
	return m_position;
}

bool PhysicsComponent::SetRotation(const math::vector3d& v)
{
	m_rotation=v;
	if(m_node)
	{
		m_node->setGlobalOrintation(math::quaternion(m_rotation));
	}
	return true;
}

const math::vector3d& PhysicsComponent::GetRotation()
{
	return m_rotation;
}

void PhysicsComponent::Update(float dt)
{
	IGameComponent::Update(dt);
	if(IsEnabled()  && m_node)
	{
		math::vector3d pos=m_node->getGlobalPosition();
		math::quaternion ori=m_node->getGlobalOrintation();
		std::list<PhysicalComponentAttachment*>::iterator it= m_attachments.begin();
		for(;it!=m_attachments.end();++it)
		{
			//update attached movable object
			(*it)->Update(pos,ori);
		}
	}
}
xml::XMLElement*  PhysicsComponent::loadXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret=IGameComponent::loadXMLSettings(elem);
	if(!ret)
		return ret;
	if(m_physicsModel==mT(""))
	{
		physics::PhysicXMLSerializer::Parse(ret,&m_nodeDesc);
		if(m_nodeDesc.bodyDesc)
		{
			m_bodyDesc=*m_nodeDesc.bodyDesc;

			//m_bodyDesc.mass*=0.1;
			m_bodyDesc.autoDelete=false;
			delete m_nodeDesc.bodyDesc;
			m_nodeDesc.bodyDesc=&m_bodyDesc;
			m_isDynamic=true;
		}else
			m_isDynamic=false;
	}
	xml::XMLElement* att=ret->getSubElement(mT("Attachments"));
	if(att)
	{
		att=att->getSubElement(mT("Attachment"));
		while(att)
		{
			PhysicalComponentAttachment* attComp=new PhysicalComponentAttachment();
			attComp->LoadFromXML(att);
			m_attachments.push_back(attComp);
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

xml::XMLElement*  PhysicsComponent::exportXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret=IGameComponent::exportXMLSettings(elem);
	
	if(m_node)
	{
		if(m_physicsModel==mT(""))
		{
			xml::XMLElement* e=new xml::XMLElement(mT("PhysicalNodeDesc"));

			physics::PhysicalNodeDesc desc;
			physics::PhysicalBodyDesc bdesc;
			m_node->SaveToDesc(&desc);
			if(m_node->SaveToDesc(&bdesc))
			{
				desc.bodyDesc=&bdesc;
			}
			else
				desc.bodyDesc=0;
			physics::PhysicXMLSerializer::Save(ret,&desc);
		}
		/*
		physics::PhysicalBodyDesc bdesc;
		if(m_node->SaveToDesc(&bdesc))
			desc.bodyDesc=&bdesc;
		else
			desc.bodyDesc=0;

		desc.SaveToXML(e);
		ret->addSubElement(e);*/
	}

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
