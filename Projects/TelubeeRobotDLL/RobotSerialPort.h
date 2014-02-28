


/********************************************************************
	created:	2013/12/04
	created:	4:12:2013   14:33
	filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\RobotSerialPort.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
	file base:	RobotSerialPort
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __RobotSerialPort__
#define __RobotSerialPort__



	struct RobotStatus;

	class RobotSerialPortImpl;
	class ITelubeeRobotListener;
class RobotSerialPort
{
protected:

	RobotSerialPortImpl* m_impl;
	int robot_control(int velocity_x, int velocity_y, int rotation, int control);
	int head_control(float pan, float tilt, float roll);

public:
	RobotSerialPort();
	virtual~RobotSerialPort();

	void SetListener(ITelubeeRobotListener* l);
	void ConnectRobot();
	void DisconnectRobot();
	bool IsConnected();
	void UpdateRobotStatus(const RobotStatus& st);
};



#endif
