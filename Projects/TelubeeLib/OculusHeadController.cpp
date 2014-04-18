

#include "stdafx.h"
#include "OculusHeadController.h"
#include "AppData.h"
#include "OculusDevice.h"

namespace mray
{
namespace TBee
{

OculusHeadController::OculusHeadController()
{
}
OculusHeadController::~OculusHeadController()
{
}


math::quaternion OculusHeadController::GetHeadOrientation()
{
	if (!AppData::Instance()->oculusDevice)
		return math::quaternion::Identity;
	return AppData::Instance()->oculusDevice->GetOrientation();
}
math::vector3d OculusHeadController::GetHeadPosition()
{
	 return math::vector3d::Zero;
}

}
}

