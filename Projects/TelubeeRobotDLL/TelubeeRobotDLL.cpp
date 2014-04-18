// TelubeeRobotDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TelubeeRobotDLL.h"

#include "RobotSerialPort.h"

#include <stdio.h>

// This is the constructor of a class that has been exported.
// see TelubeeRobotDLL.h for the class definition
CTelubeeRobotDLL::CTelubeeRobotDLL()
{
	m_impl = new RobotSerialPort();

	printf("Robot Communicator was Created!\n");
}


CTelubeeRobotDLL::~CTelubeeRobotDLL(void)
{
	delete m_impl;
	printf("Robot Communicator was Destroyed!\n");
}



void CTelubeeRobotDLL::SetListener(ITelubeeRobotListener* l)
{
	m_impl->SetListener(l);
}


void CTelubeeRobotDLL::OnRobotStatus(const RobotStatus& st)
{
	m_impl->UpdateRobotStatus(st);
}

