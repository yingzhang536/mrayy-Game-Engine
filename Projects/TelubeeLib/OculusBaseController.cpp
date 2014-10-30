

#include "stdafx.h"
#include "OculusBaseController.h"
#include "AppData.h"
#include "OculusDevice.h"

namespace mray
{
namespace TBee
{

OculusBaseController::OculusBaseController()
{
	m_currentRotation = 0;
}
OculusBaseController::~OculusBaseController()
{
}


bool OculusBaseController::IsButtonPressed(EInputButton button)
{
	return false;
}
math::vector2d OculusBaseController::GetSpeed()
{
#ifdef OCULUS_DK1
	return math::vector3d::Zero;
#else
	if (!AppData::Instance()->oculusDevice)
		return 0;
	math::vector3d p = AppData::Instance()->oculusDevice->GetPosition();
	math::vector3d diff = p - m_headPosCalib;

	float x = diff.z;
	float y = diff.x;

	float minOffset = 0.1;
	float maxOffset = 0.5;

	x = math::sign(x)*math::Min<float>(1.0f, math::Max<float>(0, abs(x) - minOffset) / (maxOffset-minOffset));
	y = math::sign(y)*math::Min<float>(1.0f, math::Max<float>(0, abs(y) - minOffset) / (maxOffset - minOffset));

	return math::vector2d(x,y);
#endif
}

float OculusBaseController::GetRotation()
{
	if (!AppData::Instance()->oculusDevice)
		return 0;

	float y = AppData::Instance()->oculusDevice->GetOrientation().getYaw();


	float minAngle = 40;
	float maxAngle = 120;

	y = math::sign(y)*math::Min<float>(1.0f, math::Max<float>(0, abs(y) - minAngle) / (maxAngle-minAngle));
	m_currentRotation = y;
	m_currentRotation = math::clamp<float>(2*m_currentRotation, -1, 1);


	float r = m_currentRotation;
	return -r;
}
void OculusBaseController::Recalibrate()
{
	if (AppData::Instance()->oculusDevice)
	{
		m_headPosCalib = AppData::Instance()->oculusDevice->GetPosition();
	}
}

}
}


