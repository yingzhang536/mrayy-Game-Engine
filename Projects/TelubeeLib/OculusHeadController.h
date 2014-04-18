

/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   20:44
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\OculusHeadController.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	OculusHeadController
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __OculusHeadController__
#define __OculusHeadController__


#include "IHeadController.h"


namespace mray
{

namespace TBee
{
class OculusHeadController :public IHeadController
{
protected:
public:
	OculusHeadController();
	virtual~OculusHeadController();

	virtual math::quaternion GetHeadOrientation();
	virtual math::vector3d GetHeadPosition();
};

}
}


#endif
