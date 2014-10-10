
#include "stdafx.h"
#include "SceneComponent.h"
#include "ISceneNode.h"
#include "GameEntityManager.h"
#include "IScenemanager.h"
#include "IModelComponent.h"


namespace mray
{
namespace game
{

SceneComponent::SceneComponent(GameEntityManager*m):m_node(0),m_removeNodeOnDestroy(0)
{
	m_sceneMngr=m->GetSceneManager();
	CPropertieDictionary* dic;
	if(CreateDictionary(&dic))
	{
	}
}
SceneComponent::SceneComponent():m_node(0),m_removeNodeOnDestroy(0),m_sceneMngr(0)
{
	CPropertieDictionary* dic;
	if(CreateDictionary(&dic))
	{
	}
}


SceneComponent::~SceneComponent()
{
	if(m_removeNodeOnDestroy)
	{
		scene::ISceneNode*node=dynamic_cast<scene::ISceneNode*>(m_node);
		if(node)
			node->getSceneManager()->removeSceneNode(node);
	}
}

bool SceneComponent::AddComponent(IObjectComponent* comp)
{
	if(!IGameComponent::AddComponent(comp))
		return false;

	IModelComponent* m=dynamic_cast<IModelComponent*>(comp);
	if(m )
	{
		m_node->AttachNode(m->GetModel());
	}
	return true;
}
void SceneComponent::RemoveComponent(IObjectComponent* comp)
{
	IModelComponent* m=dynamic_cast<IModelComponent*>(comp);
	if(m )
	{
		m_node->RemoveNode(m->GetModel());
	}
	IGameComponent::RemoveComponent(comp);
}
bool SceneComponent::InitComponent()
{
	if(!IGameComponent::InitComponent())
		return false;
	if(m_node)//already inited
		return true;
	m_node= m_sceneMngr->createSceneNode(m_nodeDesc.name);
	m_node->LoadFromDesc(&m_nodeDesc);

	std::list<IObjectComponent*>::iterator it=  m_componentList.begin();
	for(;it!=m_componentList.end();++it)
	{
		IModelComponent* comp=dynamic_cast<IModelComponent*>(*it);
		comp->InitComponent();//make sure its inited
		if(comp && comp->GetModel())
		{
			m_node->AttachNode(comp->GetModel());
		}
	}

	SceneComponent* parent=dynamic_cast<SceneComponent*>(m_ownerComponent);
	if(parent && parent->GetSceneNode())
	{
		parent->GetSceneNode()->addChild(m_node);
	}

	return true;
}
bool SceneComponent::SetEnabled(bool e)
{
	IGameComponent::SetEnabled(e);
	if (m_node)
	{
		m_node->setVisible(e,false);
	}
	return true;
}

void SceneComponent::SetSceneNode(scene::ISceneNode* node)
{
	m_node=node;
	//AddSubSet(mT("SceneNode"),m_node);
}
IObject* SceneComponent::GetAttachedObject()
{
	return m_node;
}
scene::IMovable* SceneComponent::GetMovableObject()
{
	return m_node;
}


xml::XMLElement*  SceneComponent::loadXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret=IGameComponent::loadXMLSettings(elem);
	if(!ret)
		return ret;
	xml::XMLElement* e=ret->getSubElement(mT("SceneNodeDesc"));
	if(e)
	{
		m_nodeDesc.loadFromXML(e);
	}
	return ret;
}


xml::XMLElement*  SceneComponent::exportXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret=IGameComponent::exportXMLSettings(elem);
	if(m_node)
	{
		xml::XMLElement* e=new xml::XMLElement(mT("SceneNodeDesc"));
		
		m_node->SaveToDesc(&m_nodeDesc);
		m_nodeDesc.saveToXML(e);
		ret->addSubElement(e);
	}

	return ret;
}

}
}