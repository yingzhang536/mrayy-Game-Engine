

// VTelesar5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TRApplication.h"
#include "GCCollector.h"
#include "DirectShowVideoGrabber.h"
#include <windows.h>
// #include <vld.h>
// #include <vldapi.h>


using namespace mray;
using namespace core;
using namespace math;

#define EntryPoint int main()
EntryPoint
{

	GCPtr<TRApplication> app = new TRApplication();

	core::string resFileName = mT("plugins.stg");

#ifdef UNICODE
	resFileName = mT("pluginsU.stg");
#endif

	printf("%d\n", sizeof(std::string));


	gLogManager.setVerbosLevel(EVL_Heavy);

	std::vector<SOptionElement> extraOptions;
	SOptionElement op;
	op.valueSet.clear();
	op.name = "Debugging";
	op.value = "No";
	op.valueSet.insert("Yes");
	op.valueSet.insert("No");
	extraOptions.push_back(op);
	op.valueSet.clear();
	{
		op.name = "Camera";
		video::DirectShowVideoGrabber ds;
		int camsCount = ds.ListDevices();
		for (int i = 0; i<camsCount; ++i)
		{
			op.valueSet.insert(ds.GetDeviceName(i));
		}
		if (op.valueSet.size()>0)
		{
			op.value = *op.valueSet.begin();
		}
		extraOptions.push_back(op);
		op.valueSet.clear();
	}
	//VLDEnable();
	app->loadResourceFile(mT("tbdataPath.stg"));
	if (app->startup(mT("TELUBee Robot Agent 1.00"), vector2di(800, 600), false, extraOptions, resFileName, 0, true, true, true))
	{
		app->run();
	}

	//	VLDDisable();
	app = 0;

	GCCollector::shutdown();
	return 0;
}


// TelubeeRobotAgent.cpp : Defines the entry point for the console application.
//
#if 0
#include "stdafx.h"
#include "multicast.h"
#include "tinyxml2.h"

#include <vector>

#include "DirectShowVideoGrabber.h"

UDPServer* server;

struct RobotStatus
{
	bool connected;
	float speedX, speedY;
	float rotation;
	float tilt, yaw, roll;
}status;


struct UserStatus
{
	std::string ipAddress;
	int port;
}userStatus;

/////////////////////////////////////////////////////////////////////////

void SendRobotData(const RobotStatus& status)
{
	//todo: send the data to control the robot

	printf("Speed= %f,%f\n", status.speedX, status.speedY);
	printf("Rotation = %f\n", status.rotation);
	printf("Head = %f,%f,%f\n", status.tilt, status.yaw, status.roll);
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
	if (name == "Speed" && vals.size() == 2)
	{
		status.speedX = atof(vals[0].c_str());
		status.speedY = atof(vals[1].c_str());
	}
	else if (name == "Head" && vals.size() == 3)
	{
		status.tilt = atof(vals[0].c_str());
		status.yaw = atof(vals[1].c_str());
		status.roll = atof(vals[2].c_str());
	}
	else if (name == "Rotation" && vals.size() == 1)
	{
		status.rotation = atof(vals[0].c_str());
	}
	else if (name == "User" && vals.size() == 2)
	{
		userStatus.ipAddress = vals[0];
		userStatus.port = atoi(vals[1].c_str());
	}
}


void ProcessPacket(const std::string& buffer)
{

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.Parse(buffer.c_str());
	if (err != tinyxml2::XML_NO_ERROR)
	{
		SendRobotData(status);//send update once
		return;
	}

	tinyxml2::XMLElement*root = doc.RootElement();
	status.connected = root->BoolAttribute("Connected");
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
		std::string buffer = server->readBuffer();
		if (buffer != "")
			ProcessPacket(buffer);
		Sleep(1);
	}
	return 0;
}

#endif