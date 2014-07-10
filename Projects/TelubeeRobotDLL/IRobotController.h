
#ifndef IRobotController_h__
#define IRobotController_h__


class ITelubeeRobotListener;
struct RobotStatus;

class IRobotController
{
protected:
public:
	IRobotController(){}
	virtual ~IRobotController(){}

	virtual void SetListener(ITelubeeRobotListener* l)=0;
	virtual void ConnectRobot() = 0;
	virtual void DisconnectRobot() = 0;
	virtual bool IsConnected() = 0;
	virtual void UpdateRobotStatus(const RobotStatus& st)=0;
	
};


#endif // IRobotController_h__

