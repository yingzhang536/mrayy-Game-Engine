

#include "stdafx.h"
#include "KeyboardHeadController.h"
#include "AppData.h"
#include "InputManager.h"
#include "RenderWindow.h"


namespace mray
{
namespace TBee
{

math::quaternion KeyboardHeadController::GetHeadOrientation()
{
	controllers::IMouseController* mouse= AppData::Instance()->inputMngr->getMouse();
	math::vector2d sz= AppData::Instance()->inputMngr->GetRenderWindow()->GetSize();

	float pan = mouse->getX()-sz.x/2;
	float tilt = mouse->getY() - sz.y / 2;
	pan /= sz.x;
	tilt /= sz.y;

	pan *= 70*2;
	tilt *= 50 * 2;

	return math::quaternion(tilt, pan, 0);

}
math::vector3d KeyboardHeadController::GetHeadPosition()
{
	return math::vector3d::Zero;
}

}
}

