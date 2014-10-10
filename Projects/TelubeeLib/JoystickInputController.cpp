
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
	m_currentRotation = 0;
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

	m_currentSpeed += (speed - m_currentSpeed)*gEngine.getFPS()->dt()*3;

	return m_currentSpeed;
	
}

float JoystickInputController::GetRotation()
{
	controllers::IJoysticController* joystick = AppData::Instance()->inputMngr->getJoystick(0);
	if (!joystick)
		return 0;
	
	 float r= joystick->getAxisState(JOYSTICK_Axis2).abs;

	 m_currentRotation += (r - m_currentRotation)*gEngine.getFPS()->dt() * 9;
	 return m_currentRotation;
}

}
}
