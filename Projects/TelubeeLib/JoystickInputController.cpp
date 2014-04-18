
#include "stdafx.h"
#include "JoystickInputController.h"
#include "AppData.h"
#include "JoystickDefinitions.h"



namespace mray
{
namespace TBee
{

JoystickInputController::JoystickInputController()
{

}

JoystickInputController::~JoystickInputController()
{

}



bool JoystickInputController::IsButtonPressed(EInputButton button)
{
	controllers::IJoysticController* joystick = AppData::Instance()->inputMngr->getJoystick(0);
	if (!joystick)
		return 0;
	switch (button)
	{
	case mray::TBee::EInputButton::Start:
		if (joystick->getButtonState(JOYSTICK_StartButton))
			return true;
	case mray::TBee::EInputButton::Stop:
		if (joystick->getButtonState(JOYSTICK_SelectButton))
			return true;
	case mray::TBee::EInputButton::Exit:
		if (joystick->getButtonState(JOYSTICK_TriangleButton))
			return true;
	default:
		return false;
	}

}

math::vector2d JoystickInputController::GetSpeed()
{

	controllers::IJoysticController* joystick = AppData::Instance()->inputMngr->getJoystick(0);
	if (!joystick)
		return 0;
	math::vector2d speed;
	speed.x = joystick->getAxisState(JOYSTICK_Axis0).abs;
	speed.y = joystick->getAxisState(JOYSTICK_Axis1).abs;

	return speed;
	
}

float JoystickInputController::GetRotation()
{
	controllers::IJoysticController* joystick = AppData::Instance()->inputMngr->getJoystick(0);
	if (!joystick)
		return 0;
	
	 return joystick->getAxisState(JOYSTICK_Axis2).abs;

}

}
}
