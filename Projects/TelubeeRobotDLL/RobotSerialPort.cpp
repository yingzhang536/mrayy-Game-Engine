

#include "stdafx.h"
#pragma warning(X:4005)

#include "RobotSerialPort.h"

#include "Tserial_event.h"
#include "agent.h"
#include "movingAverage.h"

#include "TelubeeRobotDLL.h"


	// used for serial sending
#define RUN		1
#define STOP	0
#define BASE	0
#define HEAD	1

	bool debug_print = false;

	class RobotSerialPortImpl
	{
	public:
		Tserial_event *comROBOT;	// Serial Port
		Tserial_event *comHEAD;	// Serial Port
		// moving avarage 
		MovAvg *mvRobot[2][3]; // 1 - base, 2 - head


		ITelubeeRobotListener* listener;

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
	ConnectRobot();
}

RobotSerialPort::~RobotSerialPort()
{
	DisconnectRobot();
	delete m_impl;
}


void RobotSerialPort::ConnectRobot()
{
	int ret=0;
	m_impl->comROBOT = new Tserial_event();
	m_impl->comHEAD = new Tserial_event();
	if (m_impl->comROBOT != 0){
		m_impl->comROBOT->setManager(robot_SerialEventManager);
		ret = m_impl->comROBOT->connect(robotCOM, robot_baudRate, SERIAL_PARITY_NONE, 8, FALSE);
		if (!ret){
			printf("Robot Connected!\n", ret);
			m_impl->comROBOT->setRxSize(15);
		}
		else{
			printf("Robot not connected (%ld)\n", ret);
			m_impl->comROBOT->disconnect();
			delete m_impl->comROBOT;
			m_impl->comROBOT = 0;
		}
	}

	if (m_impl->comHEAD != 0){
		m_impl->comHEAD->setManager(head_SerialEventManager);
		ret = m_impl->comHEAD->connect(headCOM, head_baudRate, SERIAL_PARITY_NONE, 8, FALSE);
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

int RobotSerialPort::robot_control(int velocity_x, int velocity_y, int rotation, int control){
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


int RobotSerialPort::head_control(float pan, float tilt, float roll){
	int packet_size;
	char sCommand[128];
	if (!m_impl->comHEAD)
		return FALSE;

	sprintf_s(sCommand, 128, "%3.2f,%3.2f,%3.2f,\n", pan, tilt, roll);

	packet_size = strlen(sCommand);
	m_impl->comHEAD->sendData(sCommand, packet_size);

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
	if (!IsConnected())
		return;
	//todo: send the data to control the robot
	int robot_vx, robot_vy, robot_rot;
	float pan, tilt, roll;

	int v_scale = 500;
	int r_scale = 400;


	float v_size;

	robot_vx = m_impl->mvRobot[BASE][0]->getNext(st.speedX*v_scale);
	robot_vy = m_impl->mvRobot[BASE][1]->getNext(st.speedY*v_scale);
	robot_rot = m_impl->mvRobot[BASE][2]->getNext(st.rotation*r_scale);

	pan = m_impl->mvRobot[HEAD][0]->getNext(st.roll);
	tilt = m_impl->mvRobot[HEAD][1]->getNext(st.tilt);
	roll = m_impl->mvRobot[HEAD][2]->getNext(st.yaw);

	v_size = sqrt(static_cast<double>(robot_vx)*static_cast<double>(robot_vx)+static_cast<double>(robot_vy)*static_cast<double>(robot_vy));

	if (debug_print){
		printf("Robot Speed / Rot = %f,%f,%f\n", st.speedX, st.speedY, st.rotation);
		printf("Head = %f,%f,%f\n", st.roll, st.tilt, st.yaw);
		printf("Robot Commands = %d,%d,%d,%3.2f\n", robot_vx, robot_vy, robot_rot, v_size);
		printf("-------------------------------\n");
	}

	head_control(pan, tilt, roll);

	if (v_size > 2.0)
		robot_control(-robot_vx, -robot_vy, -robot_rot, RUN);
	else
		robot_control(0, 0, 0, STOP);


	Sleep(1);
}

void RobotSerialPort::SetListener(ITelubeeRobotListener* l)
{
	m_impl->listener = l;
}

