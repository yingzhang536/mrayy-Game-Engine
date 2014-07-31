// TelubeeRobotDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NissanRobotDLL.h"
#include "RobotSerialPort.h"
#include <stdio.h>


CNissanRobotDLL*CNissanRobotDLL::instance = 0;
// This is the constructor of a class that has been exported.
// see TelubeeRobotDLL.h for the class definition
CNissanRobotDLL::CNissanRobotDLL()
{
	instance = this;
	m_impl = new RobotSerialPort();

	printf("Robot Communicator was Created!\n");
}

void CNissanRobotDLL::Destroy()
{
	((RobotSerialPort*)m_impl)->IAI_destroy();
}
CNissanRobotDLL::~CNissanRobotDLL(void)
{
	delete m_impl;
	instance = 0;
	printf("Robot Communicator was Destroyed!\n");
}




