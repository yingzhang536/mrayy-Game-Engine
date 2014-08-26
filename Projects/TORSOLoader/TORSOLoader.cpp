// TORSOLoader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TorsoRobotDLL.h"
#include "IRobotController.h"
#include <stdlib.h>
#include <thread>

#include <windows.h>
#include <iostream>

#include <cctype>
IRobotController* robot;
RobotStatus status;

bool quit = false;

void PrintStatus()
{
	printf("-----------------   Status    -------------------------------\n");
	printf("Robot status: %s\n", robot->IsConnected() ? "Connected" : "Disconnected");
	printf("-------------------------------------------------------------\n");
}

void PrintCommand()
{
	printf("-----------------   Commands List    -----------------------\n");
	printf("Commands List:\n");
	if (robot->IsConnected())
	{
		printf("D- Disconnect\n");

		printf("P- Drive position [x,y,z]\n");
		printf("R- Drive rotation [tilt,yaw,roll]\n");
	}
	else
		printf("C- Connect\n");

	printf("Q- Quit\n");
}

void ProcessCommand(char c)
{
	c = tolower(c);
	switch (c)
	{
	case 'c':
		robot->ConnectRobot();
		status.connected = true;
		break;
	case 'd':
		robot->DisconnectRobot();
		status.connected = false;
		break;
	case 'p':
	{
		if (!robot->IsConnected())
			break;
		float x, y, z;
		printf("Enter Position Values (x,y,z):");
		scanf_s("%f,%f,%f", &x, &y, &z);
		status.headPos.x = x;
		status.headPos.y = y;
		status.headPos.z = z;
	}
	break;
	case 'r':
	{
		if (!robot->IsConnected())
			break;
		float x, y, z;
		printf("Enter Rotation Values (tilt,yaw,roll):");
		scanf_s("%f,%f,%f", &x, &y, &z);
		status.headRotation.fromEulerAngles(x, y, z);
	}
		break;
	case 'q':
		quit = true;
		break;
	default:
		break;
	}
}

void ProcessThread(void* arg)
{
	while (!quit)
	{
		if (robot->IsConnected())
		{
			robot->UpdateRobotStatus(status);
		}
		Sleep(10);
	}
}
int _tmain(int argc, _TCHAR* argv[])
{

	DLL_RobotInit();
	robot = DLL_GetRobotController();

	std::thread process(ProcessThread,(void*) NULL);;
	process.detach();//start robot process

	do 
	{
		PrintStatus();
		PrintCommand();

		printf("Enter Command:");
		
		char c;
		
		do 
		{
			c = getchar();
		} while (c=='\n');
		system("cls");

		ProcessCommand(c);
		printf("\n\n");

	} while (!quit);
	robot->DisconnectRobot();
	DLL_RobotDestroy();
	return 0;
}

