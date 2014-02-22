

/********************************************************************
	created:	2014/01/08
	created:	8:1:2014   21:26
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\IInputController.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	IInputController
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __IInputController__
#define __IInputController__



#include "IDelegate.h"

namespace mray
{
namespace TBee
{
	class IInputController;
	enum class EInputButton
	{
		Start,
		Stop,
		Exit
	};
class IInputController
{
protected:
public:

	IInputController(){}
	virtual~IInputController(){}

	virtual bool IsButtonPressed(EInputButton button) = 0;
	virtual math::vector2d GetSpeed() = 0;
	virtual float GetRotation() = 0;
};

}
}


#endif
