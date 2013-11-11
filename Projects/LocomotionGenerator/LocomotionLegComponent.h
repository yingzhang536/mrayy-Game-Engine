


/********************************************************************
	created:	2012/08/25
	created:	25:8:2012   12:41
	filename: 	d:\Development\mrayEngine\Projects\LocomotionGenerator\LocomotionLeg.h
	file path:	d:\Development\mrayEngine\Projects\LocomotionGenerator
	file base:	LocomotionLeg
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___LocomotionLeg___
#define ___LocomotionLeg___

#include "IGameComponent.h"

namespace mray
{
namespace locomotion
{

	class LocoLegPartInformation
	{
	protected:
	public:
		LocoLegPartInformation()
		{
		}
	};

class LocomotionLegComponent:public game::IGameComponent
{
protected:

public:
	LocomotionLeg();
	virtual~LocomotionLeg();
	
	
};

}
}

#endif
