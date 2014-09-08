

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
	math::quaternion q = AppData::Instance()->oculusDevice->GetOrientation()*m_initial;
	q.Normalize();

	return q;
}
math::vector3d OculusHeadController::GetHeadPosition()
{
#ifdef OCULUS_DK1
	return math::vector3d::Zero;
#else
	if (!AppData::Instance()->oculusDevice)
		return 0;
	return AppData::Instance()->oculusDevice->GetPosition();

#endif
}

void OculusHeadController::Recalibrate()
{
	if (AppData::Instance()->oculusDevice)
	{
		AppData::Instance()->oculusDevice->ResetOrientation();
		m_initial = AppData::Instance()->oculusDevice->GetOrientation();
		m_initial = math::quaternion::Identity;
	}
	m_initial = m_initial.inverse();
}

}
}

