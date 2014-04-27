

#include "stdafx.h"
#include "VisItemDescComponent.h"




namespace mray
{
namespace game
{
IMPLEMENT_SETGET_PROP(VisItemDescComponent, Description, core::string, m_description, "", , );
IMPLEMENT_RTTI(VisItemDescComponent, SceneComponent)

IMPLEMENT_GAMECOMPONENT_FACTORY(VisItemDescComponent)


VisItemDescComponent::VisItemDescComponent(game::GameEntityManager* m) :SceneComponent(m)
{

	CPropertieDictionary* dic;
	if (CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeDescription::instance);
	}
}

VisItemDescComponent::~VisItemDescComponent()
{
}


bool VisItemDescComponent::InitComponent()
{
	return SceneComponent::InitComponent();
}


void VisItemDescComponent::Update(float dt)
{
	SceneComponent::Update(dt);
}

}
}