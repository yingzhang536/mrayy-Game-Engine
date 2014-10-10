
#include "stdafx.h"
#include "IGameComponent.h"
#include "GameEntity.h"
#include "IGameVisitor.h"
#include "GameComponentCreator.h"
#include "XMLTree.h"
#include "IFileSystem.h"
#include "StreamWriter.h"
#include "StreamReader.h"


namespace mray
{
namespace game
{

 
	//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Enabled,IGameComponent,bool,mT("Enabled"),EPBT_Basic,mT("Enable/Disable the component"),true);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Enabled,IGameComponent,bool,SetEnabled,IsEnabled,core::StringConverter::toString,core::StringConverter::toBool,false)

IMPLEMENT_PROPERTY_TYPE_HEADER(Name,IGameComponent,core::string,mT("Name"),EPBT_Basic,mT(""),mT(""));
IMPLEMENT_PROPERTY_TYPE_GENERIC(Name,IGameComponent,core::string,SetName,GetName,,,false);

game::IGameComponent* IGameComponent::RetriveComponent(IObjectComponent*comp, const core::string& type, const core::string& name)
{
	ComponentsMap::const_iterator it = comp->GetComponents().begin();
	for (; it != comp->GetComponents().end(); ++it)
	{
		if(!it->first->isKindOf(type))
			continue;

		std::list<IObjectComponent*>::const_iterator it2=it->second.begin();
		for (;it2!=it->second.end();++it2)
		{
			game::IGameComponent* c=dynamic_cast<game::IGameComponent*>(*it2);
			if(c->GetName().equals_ignore_case(name))
				return c;
		}
	}
	/*
	const std::list<IObjectComponent*>& lst=comp->GetComponent(type);
	std::list<IObjectComponent*>::const_iterator it=lst.begin();
	for (;it!=lst.end();++it)
	{
		game::IGameComponent* c=dynamic_cast<game::IGameComponent*>(*it);
		if(c->GetName().equals_ignore_case(name))
			return c;
	}*/
	return 0;
}

IGameComponent::IGameComponent():m_ownerEntity(0),m_enabled(true)
{
	CPropertieDictionary*dic=0;
	if(CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeEnabled::instance);
		dic->addPropertie(&PropertyTypeName::instance);
	}
	m_inited=false;
}
IGameComponent::~IGameComponent()
{
}

void IGameComponent::OnAttachedToComponent(IObjectComponent* owner)
{
	IObjectComponent::OnAttachedToComponent(owner);
	GameEntity* ent;
	ent= dynamic_cast<GameEntity*>(owner);
	if(!ent)
	{
		IGameComponent* comp=dynamic_cast<IGameComponent*>(owner);
		if(comp)
			ent=comp->GetOwnerEntity();
	}

	m_ownerEntity=ent;
}
void IGameComponent::OnRemovedFromComponent(IObjectComponent* owner)
{
	m_ownerComponent=0;
}
bool IGameComponent::InitComponent()
{
	if(m_inited)
		return false;
	m_inited=true;
	bool ret=true;
	std::list<IObjectComponent*>::iterator it= m_componentList.begin();
	for(;it!=m_componentList.end();++it)
	{
		IGameComponent* comp=dynamic_cast<IGameComponent*>(*it);
		if(comp)
			ret &=comp->InitComponent();
	}
	return ret;
}

GameEntity* IGameComponent::GetOwnerEntity()const
{
	return m_ownerEntity;
}

bool IGameComponent::SetEnabled(bool enabled)
{
	m_enabled = enabled;
	std::list<IObjectComponent*>::iterator it = m_componentList.begin();
	for (; it != m_componentList.end(); ++it)
	{
		IGameComponent* comp = dynamic_cast<IGameComponent*>(*it);
		if (comp)
			comp->SetEnabled(enabled);
	}
	return true;
}
bool IGameComponent::IsEnabled()const
{
	return m_enabled;
}

void IGameComponent::OnReset()
{
	std::list<IObjectComponent*>::iterator it= m_componentList.begin();
	for(;it!=m_componentList.end();++it)
	{
		IGameComponent* comp=dynamic_cast<IGameComponent*>(*it);
		if(comp)
			comp->OnReset();
	}
}
void IGameComponent::OnDestroy()
{
	std::list<IObjectComponent*>::iterator it= m_componentList.begin();
	for(;it!=m_componentList.end();++it)
	{
		IGameComponent* comp=dynamic_cast<IGameComponent*>(*it);
		if(comp)
			comp->OnDestroy();
	}
}

void IGameComponent::PreUpdate()
{
	//update components
	std::list<IObjectComponent*>::iterator it= m_componentList.begin();
	for(;it!=m_componentList.end();++it)
	{
		IGameComponent* comp=dynamic_cast<IGameComponent*>(*it);
		if(comp)
			comp->PreUpdate();
	}
}
void IGameComponent::Update(float dt)
{
	//update components
	std::list<IObjectComponent*>::iterator it= m_componentList.begin();
	for(;it!=m_componentList.end();++it)
	{
		IGameComponent* comp=dynamic_cast<IGameComponent*>(*it);
		if(comp)
			comp->Update(dt);
	}
}
void IGameComponent::LateUpdate(float dt)
{
	//update components
	std::list<IObjectComponent*>::iterator it= m_componentList.begin();
	for(;it!=m_componentList.end();++it)
	{
		IGameComponent* comp=dynamic_cast<IGameComponent*>(*it);
		if(comp)
			comp->LateUpdate(dt);
	}
}

void IGameComponent::DebugRender(scene::IDebugDrawManager* renderer)
{
	std::list<IObjectComponent*>::iterator it= m_componentList.begin();
	for(;it!=m_componentList.end();++it)
	{
		IGameComponent* comp=dynamic_cast<IGameComponent*>(*it);
		if(comp)
			comp->DebugRender(renderer);
	}
}
void IGameComponent::OnGUIRender(GUI::IGUIRenderer* renderer, const math::rectf& vp)
{
	std::list<IObjectComponent*>::iterator it = m_componentList.begin();
	for (; it != m_componentList.end(); ++it)
	{
		IGameComponent* comp = dynamic_cast<IGameComponent*>(*it);
		if (comp)
			comp->OnGUIRender(renderer,vp);
	}
}

void IGameComponent::Visit(IGameVisitor* visitor)
{
	visitor->OnVisit(this);
}
xml::XMLElement* IGameComponent::loadXMLSettings(xml::XMLElement* elem)
{
	xml::XMLElement* ret=IObjectComponent::loadXMLSettings(elem);

	xml::XMLElement* comps;

	xml::XMLTree t;
	//load external components
	comps=elem->getSubElement(mT("ComponentsFile"));
	while(comps)
	{
		xml::XMLAttribute* attr= comps->getAttribute(mT("Path"));
		if(attr)
		{
			core::string path=attr->value;
			attr=comps->getAttribute(mT("Prefix"));
			core::string pref;
			if(attr)
				pref=attr->value;
			OS::IStream* s= gFileSystem.openFile(path,OS::BIN_READ);
			if(s && t.load(s))
			{
				xml::XMLElement* e=t.getSubElement(mT("Components"));
				if(e)
					_LoadComponents(e,pref);
			}
			if(s)
				s->close();
		}
		comps=comps->nextSiblingElement(mT("ComponentsFile"));
	}

	//load internal components now
	comps= elem->getSubElement(mT("Components"));
	if(comps)
	{
		_LoadComponents(comps,mT(""));
	}


	return ret;
}
void IGameComponent::_LoadComponents(xml::XMLElement* comps,const core::string&pref)
{
	xml::xmlSubElementsMapIT it=comps->getElementsBegin();
	xml::xmlSubElementsMapIT end=comps->getElementsEnd();
	std::vector<IGameComponent*> loaded;
	for(;it!=end;++it)
	{
		if((*it)->GetType()!=xml::ENT_Element)
			continue;

		xml::XMLElement* e=(xml::XMLElement*)*it;
		core::string type= e->getName();
		GameEntityManager* mngr=0;
		if(m_ownerEntity)
			mngr=m_ownerEntity->GetCreator();
		IGameComponent* comp= GameComponentCreator::getInstance().CreateObject(type,mngr);
		if(comp && comp->IsUnique() && GetFirstComponent(type)!=0)
		{
			delete comp;
			continue;
		}
		if(comp)
		{
			AddComponent(comp);
			comp->loadXMLSettings(e);
			loaded.push_back(comp);
		}
	}

	//init components first
// 	for(int i=0;i<loaded.size();++i)
// 		loaded[i]->InitComponent();

	//then set prefix
	for(int i=0;i<loaded.size();++i)
		loaded[i]->SetPrefix(pref);

}


}
}
