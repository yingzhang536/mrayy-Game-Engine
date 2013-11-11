// TelubeeRobotAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "multicast.h"
#include "tinyxml2.h"

#include <vector>

UDPServer* server;

struct RobotStatus
{
	bool connected;
	float speedX, speedY;
	float tilt, yaw, roll;
}status;


/////////////////////////////////////////////////////////////////////////

void SendRobotData(const RobotStatus& status)
{
	//todo: send the data to control the robot

	printf("Speed= %f,%f\n", status.speedX, status.speedY);
	printf("Head = %f,%f,%f\n", status.tilt, status.yaw,status.roll);
	printf("-------------------------------\n");

}








///////////////////////////////////////////////////////////////////////// 
void split(const std::string& str, std::vector<std::string>& result)
{
	size_t start_pos = 0;
	size_t match_pos;
	size_t substr_length;

	while ((match_pos = str.find(',', start_pos)) != std::string::npos)
	{
		substr_length = match_pos - start_pos;

		if (substr_length > 0)
		{
			result.push_back(str.substr(start_pos, substr_length));
		}

		start_pos = match_pos + 1;
	}

	substr_length = str.length() - start_pos;

	if (substr_length > 0)
	{
		result.push_back(str.substr(start_pos, substr_length));
	}
}

void HandleData(const std::string& name, const std::string& value)
{
	std::vector<std::string> vals;
	split(value, vals);
	if (name == "Speed" && vals.size()==2)
	{
		status.speedX = atof(vals[0].c_str());
		status.speedY = atof(vals[1].c_str());
	}
	else if (name == "Head" && vals.size() == 3)
	{
		status.tilt = atof(vals[0].c_str());
		status.yaw  = atof(vals[1].c_str());
		status.roll = atof(vals[2].c_str());
	}
}


void ProcessPacket(const std::string& buffer)
{

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err= doc.Parse(buffer.c_str());
	if (err != tinyxml2::XML_NO_ERROR)
	{
		SendRobotData(status);//send update once
		return;
	}

	tinyxml2::XMLElement*root= doc.RootElement();
	status.connected= root->BoolAttribute("Connected");
	if (!status.connected)
		return;
	tinyxml2::XMLElement* node = root->FirstChildElement("Data");
	while (node)
	{
		std::string name = node->Attribute("N");//name of the value
		std::string value = node->Attribute("V");//name of the value
		HandleData(name, value);
		node = node->NextSiblingElement("Data");
	}

	SendRobotData(status);
}

int _tmain(int argc, _TCHAR* argv[])
{
	server = new UDPServer(20000);
	server->startRecving();
	while (true)
	{
		std::string buffer=server->readBuffer();
		if (buffer!="")
			ProcessPacket(buffer);
		Sleep(1);
	}
	return 0;
}

