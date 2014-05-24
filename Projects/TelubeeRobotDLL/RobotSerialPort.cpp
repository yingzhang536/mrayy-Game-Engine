

#include "stdafx.h"
#pragma warning(X:4005)

#include "RobotSerialPort.h"
#include "Tserial_event.h"
#include "agent.h"
#include "movingAverage.h"
#include "TelubeeRobotDLL.h"

float testPosx = 100.00;
float testPosy = 100.00; 

	// used for serial sending
#define RUN		1
#define STOP	0
#define BASE	0
#define HEAD	1

#define ROBOT_CENTER 325

bool debug_print = false;
bool threadStart = false;
bool isDone = false;
bool upCount = true;


DWORD RobotSerialPort::timerThreadHead(RobotSerialPort *robot, LPVOID pdata){
	int count = 0;
	while (!isDone){
		if (threadStart){
			//printf("thread h: %d \r", count++);	
			robot->head_control(-robot->pan, -robot->tilt, -robot->roll);
/*
			if(upCount)
				robot->head_control(count+=1, 0, 0);
			else
				robot->head_control(count-=1, 0, 0);


			if (count > 80)
				upCount = false;
			else if (count < -80)
				upCount = true;*/
		}

		
		Sleep(3);
	}
	return 0;
}


DWORD RobotSerialPort::timerThreadBase(RobotSerialPort *robot, LPVOID pdata){
	int count = 0;
	while (true){
		if (threadStart){
			//printf("thread b: %d \r", count++);
			//robot->yamahaXY_control(robot->robotX, robot->robotY, RUN);
		}

		Sleep(40);
	}
}


class RobotSerialPortImpl
	{
	public:
		Tserial_event *comROBOT;	// Serial Port
		Tserial_event *comHEAD;		// Serial Port
		MovAvg *mvRobot[2][3];		// 1 - base, 2 - head moving avarage 

		ITelubeeRobotListener* listener;
		RobotSerialPortImpl()
		{
			comROBOT = 0;
			comHEAD = 0;
			listener = 0;
		}
		void NotifyCollision(float l, float r)
		{
			if (listener)
			{
				listener->OnCollisionData(l, r);
			}
		}
};

	void robot_OnDataArrival(int size, char *buffer){

		//todo:handle return
		//comROBOT->sendData("S", 2);

	}

	void head_OnDataArrival(int size, char *buffer){

		//todo:handle return
		//comROBOT->sendData("S", 2);

	}


	void robot_SerialEventManager(uint32 object, uint32 event){
		char *buffer;
		int   size;
		Tserial_event *com;

		com = (Tserial_event *)object;
		if (com != 0){
			switch (event){
			case  SERIAL_CONNECTED:
				//printf("Com Port Connected! \n");
				break;
			case  SERIAL_DATA_ARRIVAL:
				size = com->getDataInSize();
				buffer = com->getDataInBuffer();
				robot_OnDataArrival(size, buffer);
				com->dataHasBeenRead();
				break;
			}
		}


	}


	void head_SerialEventManager(uint32 object, uint32 event){
		char *buffer;
		int   size;
		Tserial_event *com;

		com = (Tserial_event *)object;
		if (com != 0){
			switch (event){
			case  SERIAL_CONNECTED:
				break;
			case  SERIAL_DATA_ARRIVAL:
				size = com->getDataInSize();
				buffer = com->getDataInBuffer();
				head_OnDataArrival(size, buffer);
				com->dataHasBeenRead();
				break;
			}
		}


	}


RobotSerialPort::RobotSerialPort()
{

	m_impl = new RobotSerialPortImpl();
	m_impl->listener = 0;
	load_parameters();

	for (int i = 0; i < 3; i++){
		m_impl->mvRobot[BASE][i] = new MovAvg();
		m_impl->mvRobot[HEAD][i] = new MovAvg();
	}

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)timerThreadHead, this, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&timerThreadBase, this, NULL, NULL);

	ConnectRobot();

}

RobotSerialPort::~RobotSerialPort()
{
	DisconnectRobot();
	delete m_impl;
	isDone = true;
}


void RobotSerialPort::ConnectRobot()
{

	int ret=0;
	m_impl->comROBOT = new Tserial_event();
	m_impl->comHEAD = new Tserial_event();
	if (m_impl->comROBOT != 0){
		m_impl->comROBOT->setManager(robot_SerialEventManager);
		ret = m_impl->comROBOT->connect(robotCOM, robot_baudRate, SERIAL_PARITY_ODD, 8, FALSE, TRUE);
		if (!ret){
			printf("Robot Connected!\n", ret);
			m_impl->comROBOT->setRxSize(15);
			yamahaInitialize();		// initialize yamaha robot
		}
		else{
			printf("Robot not connected (%ld)\n", ret);
			printf("Robot baud (%ld)\n", ret);
			m_impl->comROBOT->disconnect();
			delete m_impl->comROBOT;
			m_impl->comROBOT = 0;
		}
	}

	if (m_impl->comHEAD != 0){
		m_impl->comHEAD->setManager(head_SerialEventManager);
		ret = m_impl->comHEAD->connect(headCOM, head_baudRate, SERIAL_PARITY_NONE, 8, FALSE, FALSE);
		if (!ret){
			printf("Head Connected!\n", ret);
			m_impl->comHEAD->setRxSize(15);
		}
		else{
			printf("Head not connected (%ld)\n", ret);
			m_impl->comHEAD->disconnect();
			delete m_impl->comHEAD;
			m_impl->comHEAD = 0;
		}
	}

}

bool RobotSerialPort::IsConnected()
{
	return m_impl->comHEAD != 0 || m_impl->comROBOT != 0;
}

void RobotSerialPort::DisconnectRobot()
{
	int ret = 0;
	printf("Disconnecting Robot\n", ret);
	if (m_impl->comROBOT != 0){
		m_impl->comROBOT->disconnect();
	}

	if (m_impl->comHEAD != 0){
		m_impl->comHEAD->disconnect();
	}
	delete m_impl->comROBOT;
	delete m_impl->comHEAD;
	m_impl->comROBOT = 0;
	m_impl->comHEAD = 0;


}

int RobotSerialPort::omni_control(int velocity_x, int velocity_y, int rotation, int control){
	int packet_size;
	char sCommand[128];
	if (!m_impl->comROBOT)
		return FALSE;

	if (control == RUN)
		sprintf_s(sCommand,128, "V,%d,%d,%d\r\n", velocity_x, velocity_y, rotation);
	else if (control == STOP)
		sprintf_s(sCommand, 128, "q\r\n");

	packet_size = strlen(sCommand);
	m_impl->comROBOT->sendData(sCommand, packet_size);

	return true;

}


int RobotSerialPort::yamahaXY_control(float pos_x, float pos_y, int control){
	int packet_size;
	char sCommand[128];

	if (!m_impl->comROBOT)
		return FALSE;

	if (control == RUN)
		sprintf_s(sCommand, 128, "@DRIVE(1,%3.2f),(2,%3.2f),XY\r\n", pos_x, pos_y);

	else if (control == STOP)
		sprintf_s(sCommand, 128, "@SERVO OFF\r\n");

	packet_size = strlen(sCommand);
	m_impl->comROBOT->sendData(sCommand, packet_size);
	printf(sCommand);

	//Sleep(40); 

	return true;

}

int RobotSerialPort::yamahaInitialize(){
	char sCommand[128];
	if (!m_impl->comROBOT)
		return FALSE;

	sprintf_s(sCommand, 128, "@TORQUE(1)=0\r\n");
	m_impl->comROBOT->sendData(sCommand, strlen(sCommand));

	sprintf_s(sCommand, 128, "@TORQUE(2)=0\r\n");
	m_impl->comROBOT->sendData(sCommand, strlen(sCommand));

	sprintf_s(sCommand, 128, "@TRQTIME(1)=10\r\n");
	m_impl->comROBOT->sendData(sCommand, strlen(sCommand));

	sprintf_s(sCommand, 128, "@TRQTIME(2)=10\r\n");
	m_impl->comROBOT->sendData(sCommand, strlen(sCommand));

	return true;

}


int RobotSerialPort::head_control(float pan, float tilt, float roll){
	int packet_size;
	char sCommand[128];
	if (!m_impl->comHEAD)
		return FALSE;

	sprintf_s(sCommand, 128, "%3.2f,%3.2f,%3.2f,\n", pan, tilt, roll);

	packet_size = strlen(sCommand);
	if (m_impl->comHEAD)
		m_impl->comHEAD->sendData(sCommand, packet_size);
	printf(sCommand);

	return true;

}



void RobotSerialPort::UpdateRobotStatus(const RobotStatus& st)
{
	if (IsConnected() && !st.connected)
	{
		DisconnectRobot();
	}
	else if (!IsConnected() && st.connected)
	{
		ConnectRobot();
	}

	if (st.connected)
		threadStart = true;
	else
		threadStart = false;

	if (!IsConnected())
		return;

	//todo: send the data to control the robot
	int robot_vx, robot_vy, robot_rot;

	int v_scale = 500;
	int r_scale = 400;

	float v_size;

	//robot_vx = m_impl->mvRobot[BASE][0]->getNext(st.speedX*v_scale);
	//robot_vy = m_impl->mvRobot[BASE][1]->getNext(st.speedY*v_scale);
	//robot_rot = m_impl->mvRobot[BASE][2]->getNext(st.rotation*r_scale);

	robotX = m_impl->mvRobot[BASE][0]->getNext(st.X * 1000);
	robotY = m_impl->mvRobot[BASE][1]->getNext(st.Z * 1000);

	pan = m_impl->mvRobot[HEAD][0]->getNext(st.yaw);
	tilt = m_impl->mvRobot[HEAD][1]->getNext(st.tilt);
	roll = m_impl->mvRobot[HEAD][2]->getNext(st.roll);

	robotX = ROBOT_CENTER - robotX;

	if (robotY > 0)
		robotY = 100.0; 
	else
		robotY = 100.0 - robotY; 

	//v_size = sqrt(static_cast<double>(robot_vx)*static_cast<double>(robot_vx)+static_cast<double>(robot_vy)*static_cast<double>(robot_vy));

	if (debug_print){
		//printf("Robot Speed / Rot = %3.2f,%3.2f,%3.2f\n", st.speedX, st.speedY, st.rotation);
		printf("Head Position = %3.2f,%3.2f\n", robotX, robotY);
		printf("Head Orientation = %3.2f,%3.2f,%3.2f\n", st.yaw, st.tilt, st.roll);
		printf("-------------------------------\n");
	}


	//head_control(pan, tilt, roll);
	//yamahaXY_control(robotx, roboty, RUN);

	//while (true){
	//	if (testPosx < 200){
	//		testPosx += 0.1;
	//		testPosy += 0.1;
	//	}
	//	else{
	//		testPosx = 50.00;
	//		testPosy = 50.00;
	//	}

	//	yamahaXY_control(testPosx, testPosy, RUN);	// init position

	//}

	//Sleep(1);

}


void RobotSerialPort::SetListener(ITelubeeRobotListener* l)
{
	m_impl->listener = l;
}

