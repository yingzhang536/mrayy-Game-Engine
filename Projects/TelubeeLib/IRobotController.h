
#ifndef IRobotController_h__
#define IRobotController_h__

#include "point2d.h"
#include "point3d.h"
#include "quaternion.h"


class ITelubeeRobotListener;
struct RobotStatus;


struct RobotStatus
{
	bool connected;
	mray::math::vector2d speed;
	float rotation;
	mray::math::quaternion headRotation;// float tilt, yaw, roll;	//head rotation
	mray::math::vector3d headPos;			//head position

	RobotStatus()
	{
		connected = false;
		rotation = 0;
	}
};
class ITelubeeRobotListener
{
public:

	virtual void OnCollisionData(float left, float right){}
};

class IRobotController
{
protected:
public:
	IRobotController(){}
	virtual ~IRobotController(){}

	virtual void SetListener(ITelubeeRobotListener* l) = 0;
	virtual void ConnectRobot() = 0;
	virtual void DisconnectRobot() = 0;
	virtual bool IsConnected() = 0;
	virtual void UpdateRobotStatus(const RobotStatus& st) = 0;

};


#endif // IRobotController_h__

