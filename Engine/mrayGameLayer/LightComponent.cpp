

#include "stdafx.h"
#include "LightComponent.h"
#include "LightNode.h"
#include "GameEntityManager.h"
#include "IScenemanager.h"
#include "IModelComponent.h"



namespace mray
{
namespace game
{

LightComponent::LightComponent(GameEntityManager*m) :m_node(0), m_removeNodeOnDestroy(0)
{
	m_sceneMngr = m->GetSceneManager();
	CPropertieDictionary* dic;
	if (CreateDictionary(&dic))
	{
	}
}

LightComponent::~LightComponent()
{
	if (m_removeNodeOnDestroy)
	{
		scene::ISceneNode*node = dynamic_cast<scene::ISceneNode*>(m_node);
		if (node)
			node->getSceneManager()->removeSceneNode(node);
	}
}

bool LightComponent::AddComponent(IObjectComponent* comp)
{
	if (!IGameComponent::AddComponent(comp))
		return false;

	IModelComponent* m = dynamic_cast<IModelComponent*>(comp);
	if (m)
	{
		m_node->AttachNode(m->GetModel());
	}
	return true;
}
void LightComponent::RemoveComponent(IObjectComponent* comp)
{
	IModelComponent* m = dynamic_cast<IModelComponent*>(comp);
	if (m)
	{
		m_node->RemoveNode(m->GetModel());
	}
	IGameComponent::RemoveComponent(comp);
}
bool LightComponent::InitComponent()
{
	if (!IGameComponent::InitComponent())
		return false;
	if (m_node)//already inited
		return true;
	m_node = m_sceneMngr->createLightNode(m_nodeDesc.name);
	m_node->LoadFromDesc(&m_nodeDesc);

	std::list<IObjectComponent*>::iterator it = m_componentList.begin();
	for (; it != m_componentList.end(); ++it)
	{
		IModelComponent* comp = dynamic_cast<IModelComponent*>(*it);
		comp->InitComponent();//make sure its inited
		if (comp && comp->GetModel())
		{
			m_node->AttachNode(comp->GetModel());
		}
	}

	return true;
}

void LightComponent::SetLightNode(scene::LightNode* node)
{
	m_node = node;
	//AddSubSet(mT("SceneNode"),m_node);
}
IObject* LightComponent::GetAttachedObject()
{
	return m_node;
}
scene::IMovable* LightComponent::GetMovableObject()
{
	return m_node;
}


xml::XMLElement*  LightComponent::loadXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret = IGameComponent::loadXMLSettings(elem);
	if (!ret)
		return ret;
	xml::XMLElement* e = ret->getSubElement(mT("LightNodeDesc"));
	if (e)
	{
		m_nodeDesc.loadFromXML(e);
	}
	return ret;
}


xml::XMLElement*  LightComponent::exportXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret = IGameComponent::exportXMLSettings(elem);
	if (m_node)
	{
		xml::XMLElement* e = new xml::XMLElement(mT("LightNodeDesc"));

		m_node->SaveToDesc(&m_nodeDesc);
		m_nodeDesc.saveToXML(e);
		ret->addSubElement(e);
	}

	return ret;
}

}
}

