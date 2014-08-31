#ifndef __torsoController__
#define __torsoController__

#include <string>
#include "IRobotController.h"
#include "vectors.h"

class torsoControllerImpl;
class torsoController :public IRobotController
{
protected:

	double targetRotMat[16];
	torsoControllerImpl* m_impl;
	float robotX, robotY, robotZ;
	int robot_vx, robot_vy, robot_rot;
	float pan, tilt, roll;
	bool baseConnected;

	bool m_connectFlag;
	bool m_isConnected;

	float m_headPos[3];

	float m_offset[3];

	IRobotStatusProvider* m_robotStatusProvider;

	static DWORD WINAPI timerThreadHead(torsoController *robot, LPVOID pdata);
	static DWORD WINAPI timerThreadBase(torsoController *robot, LPVOID pdata);

	std::string ScanePorts();

	int FirstMoving(void); 
	int FinishMoving(void); 
	int mainRoutine(int CalibSelect);
	int debugRoutine(void);

	void SetZeroPos();

public:
	torsoController();
	virtual~torsoController();

	virtual void InitializeRobot(IRobotStatusProvider* robotStatusProvider) ;
	void SetListener(ITelubeeRobotListener* l);
	int initRobot(bool debug);
	void ConnectRobot();
	void DisconnectRobot();
	bool IsConnected();
	void UpdateRobotStatus(const RobotStatus& st);


	static void ConvertToMatrix(const Quaternion& q, const float* pos, double* mat);

};



#endif
