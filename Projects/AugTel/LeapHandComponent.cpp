

#include "stdafx.h"
#include "LeapHandComponent.h"
#include "IMovableComponent.h"
#include "KinematicJointDOF6Component.h"
#include "CoupledJointComponent.h"
#include "ISceneNode.h"
#include "LeapHand.h"
#include "LeapFinger.h"


namespace mray
{
namespace AugTel
{


LeapHandComponent::LeapHandComponent(game::GameEntityManager*m):
ArmComponent(m)
{
	CPropertieDictionary* dic;
	if (CreateDictionary(&dic))
	{
	}

	m_Hand = 0;
}
LeapHandComponent::~LeapHandComponent()
{
}
bool LeapHandComponent::InitComponent()
{
	if (!ArmComponent::InitComponent())
		return false;
	SetHand(m_Hand);
	return true;
}
void LeapHandComponent::Update(float dt)
{
	ArmComponent::Update(dt);

}

void LeapHandComponent::SetHand(LeapHand*h)
{
	m_Hand = h;

	if (m_Hand)
	{
		if (m_foream)
			m_Hand->SetForeamNode(m_foream->GetMovableObject());

		if (m_wrist)
			m_Hand->SetWristJoint(m_wrist->GetMovableObject());
		
		for (int i = 0; i < 5; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (m_fingers[i][j])
					m_Hand->GetFinger((ELeapFinger)(i))->SetNode(m_fingers[i][j]->GetMovableObject(), j+1);
			}
		}
	}
}



}
}





