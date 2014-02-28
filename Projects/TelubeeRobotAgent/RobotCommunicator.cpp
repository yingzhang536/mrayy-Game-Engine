
#include "stdafx.h"
#include "RobotCommunicator.h"
#include "INetwork.h"
#include "IThreadFunction.h"
#include "IThreadManager.h"

#include "tinyxml2.h"
#include "StringUtil.h"

namespace mray
{


	class RobotCommunicatorThread :public OS::IThreadFunction
	{
		RobotCommunicator* m_owner;
	public:
		RobotCommunicatorThread(RobotCommunicator* o)
		{
			m_owner = o;
		}
		virtual void execute(OS::IThread*caller, void*arg)
		{
			while (caller->isActive())
			{
				if (m_owner->_Process()!=0)
				{
					return;
				}
			}
		}

	};

RobotCommunicator::RobotCommunicator()
{
	m_listener = 0;
	m_client = network::INetwork::getInstance().createUDPClient();
	m_thread = 0;
	

	m_localControl = 0;
	m_robotController = new CTelubeeRobotDLL();

	m_msgSink = 0;
		
}

RobotCommunicator::~RobotCommunicator()
{
	StopServer();
	delete m_client;
	delete m_robotController;
}

void RobotCommunicator::HandleData(network::NetAddress* addr,const core::string& name, const core::string& value)
{

	std::vector<core::string> vals;
	vals=core::StringUtil::Split(value, ",");

	if (name == "Speed" && vals.size() == 2)
	{
		m_robotStatus.speedX = atof(vals[0].c_str());
		m_robotStatus.speedY = atof(vals[1].c_str());
		//limit the speed
		m_robotStatus.speedX = math::clamp<float>(m_robotStatus.speedX, -1, 1);
		m_robotStatus.speedY = math::clamp<float>(m_robotStatus.speedY, -1, 1);
	}
	else if (name == "HeadRotation" && vals.size() == 3)
	{
		m_robotStatus.tilt = atof(vals[0].c_str());
		m_robotStatus.yaw = atof(vals[1].c_str());
		m_robotStatus.roll = atof(vals[2].c_str());

		//do head limits
		m_robotStatus.tilt = math::clamp(m_robotStatus.tilt, -50.0f, 50.0f);
		m_robotStatus.yaw = math::clamp(m_robotStatus.yaw, -70.0f, 70.0f);
		m_robotStatus.roll = math::clamp(m_robotStatus.roll, -40.0f, 40.0f);
	}
	else if (name == "HeadPosition" && vals.size() == 3)
	{
		m_robotStatus.X = atof(vals[0].c_str());
		m_robotStatus.Y = atof(vals[1].c_str());
		m_robotStatus.Z = atof(vals[2].c_str());

	}
	else if (name == "Rotation" && vals.size() == 1)
	{
		m_robotStatus.rotation = atof(vals[0].c_str());
		m_robotStatus.rotation = math::clamp<float>(m_robotStatus.rotation, -1, 1);
	}
	else if (name == "Connect" && vals.size() == 3)
	{
		int videoPort = atoi(vals[1].c_str());
		int audioPort = atoi(vals[2].c_str());
		network::NetAddress addr = network::NetAddress(vals[0], videoPort);
		//if (addr.address != m_userStatus.address.address || addr.port!=m_userStatus.address.port)
		{
			m_userStatus.address = addr;
			if (m_listener)
			{
				m_listener->OnUserConnected(this, m_userStatus.address,videoPort,audioPort);
			}
		}
	}
	else if (name == "Disconnect" && vals.size() == 2)
	{
		network::NetAddress addr = network::NetAddress(vals[0], atoi(vals[1].c_str()));
		if (addr.address == m_userStatus.address.address)
		{
			if (m_listener)
			{
				m_listener->OnUserDisconnected(this, m_userStatus.address);
			}
		}
	}
	else
	{
		if (m_msgSink)
			m_msgSink->OnMessage(addr,name, value);
	}
}

void RobotCommunicator::SetRobotData(const RobotStatus &st)
{
	if (m_localControl)
		_RobotStatus(st);
}

void RobotCommunicator::_RobotStatus(const RobotStatus& st)
{
	m_robotController->OnRobotStatus(st);
	if (m_listener)
		m_listener->OnRobotStatus(this, st);
}
void RobotCommunicator::ProcessPacket(network::NetAddress* addr,const char* buffer)
{

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.Parse(buffer);
	if (err != tinyxml2::XML_NO_ERROR)
	{
		//send update once
		if (!m_localControl)
			_RobotStatus(m_robotStatus);
		return;
	}
	tinyxml2::XMLElement*root = doc.RootElement();

	m_robotStatus.connected  = root->BoolAttribute("Connected");;
// 	if (!c && !m_robotStatus.connected)
// 		return;
// 	if (!m_robotStatus.connected)
// 		return;
	tinyxml2::XMLElement* node = root->FirstChildElement("Data");
	while (node)
	{
		std::string name = node->Attribute("N");//name of the value
		std::string value = node->Attribute("V");//name of the value
		HandleData(addr,name, value);
		node = node->NextSiblingElement("Data");
	}
	if (!m_localControl)
		_RobotStatus(m_robotStatus);
}

int RobotCommunicator::_Process()
{
	if (!m_client->IsOpen())
		return -1;
#define MAX_BUFFER 4096*4
	char buffer[MAX_BUFFER];
	network::NetAddress src;
	uint len = MAX_BUFFER;
	if (m_client->RecvFrom(buffer, &len, &src) != network::UDP_SOCKET_ERROR_NONE)
		return 0;//failed to receive the packet

	ProcessPacket(&src,buffer);
	return 0;
}


void RobotCommunicator::StartServer(int port)
{
	StopServer();
	m_client->Open(port);
	m_thread = OS::IThreadManager::getInstance().createThread(new RobotCommunicatorThread(this)); 
	m_thread->start(0);
	printf("Communication Channel started - Port : %d\n", port);
}

void RobotCommunicator::StopServer()
{
	m_client->Close();
	OS::IThreadManager::getInstance().killThread(m_thread);
	delete m_thread;
	m_thread = 0;
	printf("Communication Channel closed\n");

}

void RobotCommunicator::OnCollisionData(float left, float right)
{
	if (m_listener)
	{
		m_listener->OnCollisionData(this,left, right);
	}
}

}
