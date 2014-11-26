
#include "stdafx.h"
#include "ATGameComponents.h"
#include "HeadCameraComponent.h"
#include "ArmComponent.h"
#include "LeapHandComponent.h"
#include "ArmsTrajectoryGenerator.h"
#include "CompositeBoneComponent.h"
#include "FingerTipComponent.h"
#include "ArmExtenderComponent.h"
#include "PenComponent.h"
#include "TriggerObjectComponent.h"
#include "RobotSpaceComponent.h"
#include "ForwardArrowComponent.h"

#include "GameComponentCreator.h"

#include "VisItemDescComponent.h"

namespace mray
{
	using namespace  game;

namespace AugTel
{

	IMPLEMENT_RTTI(ArmComponent, IGameComponent);
	IMPLEMENT_GAMECOMPONENT_FACTORY(ArmComponent);


	IMPLEMENT_RTTI(ArmExtenderComponent, ArmComponent);
	IMPLEMENT_GAMECOMPONENT_FACTORY(ArmExtenderComponent);

	IMPLEMENT_RTTI(LeapHandComponent, ArmComponent);
	IMPLEMENT_GAMECOMPONENT_FACTORY(LeapHandComponent);

	IMPLEMENT_RTTI(PenComponent, MountableComponent);
	IMPLEMENT_GAMECOMPONENT_FACTORY(PenComponent);

	IMPLEMENT_RTTI(TriggerObjectComponent, IGameComponent);
	IMPLEMENT_GAMECOMPONENT_FACTORY(TriggerObjectComponent);

	IMPLEMENT_RTTI(RobotSpaceComponent, IGameComponent);
	IMPLEMENT_GAMECOMPONENT_FACTORY(RobotSpaceComponent);

	IMPLEMENT_RTTI(ForwardArrowComponent, IGameComponent);
	IMPLEMENT_GAMECOMPONENT_FACTORY(ForwardArrowComponent);

void ATGameComponents::RegisterComponents()
{

	REGISTER_COMPONENT_FACTORY(HeadCameraComponent);
	REGISTER_COMPONENT_FACTORY(ArmComponent);
	REGISTER_COMPONENT_FACTORY(ArmsTrajectoryGenerator);
	REGISTER_COMPONENT_FACTORY(CompositeBoneComponent);
	REGISTER_COMPONENT_FACTORY(FingerTipComponent);
	REGISTER_COMPONENT_FACTORY(PenComponent);
	REGISTER_COMPONENT_FACTORY(ArmExtenderComponent);
	REGISTER_COMPONENT_FACTORY(LeapHandComponent);
	REGISTER_COMPONENT_FACTORY(RobotSpaceComponent);
	REGISTER_COMPONENT_FACTORY(TriggerObjectComponent);
	REGISTER_COMPONENT_FACTORY(VisItemDescComponent);
	REGISTER_COMPONENT_FACTORY(ForwardArrowComponent);
	game::GameComponentCreator::getInstance().AddAlias("KinematicJointDOF6Component", "PhysicalJointComponent");
	game::GameComponentCreator::getInstance().AddAlias("NullPhysicsComponent", "RigidBodyComponent");

}

}
}


