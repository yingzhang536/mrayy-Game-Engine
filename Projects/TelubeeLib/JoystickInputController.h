

/********************************************************************
	created:	2014/02/12
	created:	12:2:2014   21:35
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\JoystickInputController.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	JoystickInputController
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __JoystickInputController__
#define __JoystickInputController__


#include "IInputController.h"

namespace mray
{
namespace TBee
{

class JoystickInputController:public IInputController
{
protected:
	math::vector2d m_currentSpeed;
	float m_currentRotation;
public:
	JoystickInputController();
	virtual~JoystickInputController();


	virtual bool IsButtonPressed(EInputButton button) ;
	virtual math::vector2d GetSpeed() ;
	virtual float GetRotation() ;
};

}
}


#endif
