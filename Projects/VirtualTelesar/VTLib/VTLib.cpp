// VTLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


#include "GameComponentCreator.h"
#include "CoupledJointComponent.h"
#include "HierarchyComponent.h"
#include "RobotCommunicatorComponent.h"
#include "CommunicationData.h"
#include "CommunicatorScheme.h"
#include "PhysicsTransformComponent.h"
#include "CommunicationManager.h"
#include "DistanceCalculatorComponent.h"
#include "CommunicationComponent.h"
#include "TouchSensorComponent.h"
#include "SpringTouchSensorComponent.h"
#include "ErrorCalculatorComponent.h"
#include "SoundListenerComponent.h"
#include "CameraComponent.h"
#include "LinkDebugComponent.h"
#include "KinBodyControlComponent.h"
#include "GGComponent.h"
#include "ContactDisablerComponent.h"


namespace mray
{
namespace VT
{
	static bool s_Inited=false;

void InitVTLib()
{
	if(s_Inited)
		return;
	s_Inited=true;
	REGISTER_COMPONENT_FACTORY(CoupledJointComponent);
	REGISTER_COMPONENT_FACTORY(RobotCommunicatorComponent);
	REGISTER_COMPONENT_FACTORY(PhysicsTransformComponent);
	REGISTER_COMPONENT_FACTORY(MountableComponent);
	REGISTER_COMPONENT_FACTORY(DistanceCalculatorComponent);
	REGISTER_COMPONENT_FACTORY(CommunicationComponent);
	REGISTER_COMPONENT_FACTORY(TouchSensorComponent);
	REGISTER_COMPONENT_FACTORY(ErrorCalculatorComponent);
	REGISTER_COMPONENT_FACTORY(SoundListenerComponent);
	REGISTER_COMPONENT_FACTORY(CameraComponent);
	REGISTER_COMPONENT_FACTORY(SpringTouchSensorComponent);
	REGISTER_COMPONENT_FACTORY(LinkDebugComponent);
	REGISTER_COMPONENT_FACTORY(KinBodyControlComponent);
	REGISTER_COMPONENT_FACTORY(GGComponent);
	REGISTER_COMPONENT_FACTORY(ContactDisablerComponent);;

	new VT::CommunicationManager();
}
void ShutdownVTLib()
{
	if(!s_Inited)
		return;
	s_Inited=false;
	delete VT::CommunicationManager::getInstancePtr();


	UNREGISTER_COMPONENT_FACTORY(CoupledJointComponent);

	UNREGISTER_COMPONENT_FACTORY(RobotCommunicatorComponent);
	UNREGISTER_COMPONENT_FACTORY(PhysicsTransformComponent);
	UNREGISTER_COMPONENT_FACTORY(MountableComponent);
	UNREGISTER_COMPONENT_FACTORY(DistanceCalculatorComponent);
	UNREGISTER_COMPONENT_FACTORY(CommunicationComponent);
	UNREGISTER_COMPONENT_FACTORY(TouchSensorComponent);
	UNREGISTER_COMPONENT_FACTORY(ErrorCalculatorComponent);
	UNREGISTER_COMPONENT_FACTORY(SoundListenerComponent);
	UNREGISTER_COMPONENT_FACTORY(CameraComponent);
	UNREGISTER_COMPONENT_FACTORY(SpringTouchSensorComponent);
	UNREGISTER_COMPONENT_FACTORY(LinkDebugComponent);
	UNREGISTER_COMPONENT_FACTORY(KinBodyControlComponent);
	UNREGISTER_COMPONENT_FACTORY(GGComponent);
	UNREGISTER_COMPONENT_FACTORY(ContactDisablerComponent);

}

}
}