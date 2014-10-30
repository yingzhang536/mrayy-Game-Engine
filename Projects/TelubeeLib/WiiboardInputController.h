

/********************************************************************
	created:	2014/02/12
	created:	12:2:2014   21:32
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\WiiboardInputController.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	WiiboardInputController
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __WiiboardInputController__
#define __WiiboardInputController__


#include "IInputController.h"


namespace mray
{
namespace TBee
{

class WiiboardInputController :public IInputController
{
protected:

	math::vector2d m_vals[4];

	math::vector2d _CalcSpeedVector();
public:
	WiiboardInputController();
	virtual~WiiboardInputController();

	virtual bool IsButtonPressed(EInputButton button);
	virtual math::vector2d GetSpeed();
	virtual float GetRotation();
	void Recalibrate(){}
};

}
}


#endif
