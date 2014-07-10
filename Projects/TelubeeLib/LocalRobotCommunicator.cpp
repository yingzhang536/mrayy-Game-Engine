
#include "stdafx.h"
#include "LocalRobotCommunicator.h"

#include "TelubeeRobotDLL.h"
#include "IRobotController.h"
#include "StringUtil.h"


namespace mray
{
namespace TBee
{

class LocalRobotCommunicatorImpl 
{
protected:
	CTelubeeRobotDLL* m_robot;
	RobotStatus m_robotStatus;
public:
	LocalRobotCommunicatorImpl()
	{
		m_robot = new CTelubeeRobotDLL();
	}
	virtual~LocalRobotCommunicatorImpl()
	{
		delete m_robot;
	}


	virtual bool Connect(const core::string& ip, int port)
	{
		m_robotStatus.connected = true;
		return true;
	}

	virtual void Disconnect()
	{
		m_robotStatus.connected = false;
	}
	virtual bool IsConnected()
	{
		return m_robotStatus.connected;
	}

	virtual void SetUserID(const core::string& userID)
	{
	}
	virtual void ConnectUser(bool c)
	{
	}
	virtual void ConnectRobot(bool c)
	{
		m_robotStatus.connected = c;
		if (m_robotStatus.connected)
			m_robot->GetRobotController()->ConnectRobot();
		else
			m_robot->GetRobotController()->DisconnectRobot();
	}

	virtual void Update(float dt)
	{
	}

	virtual void LoadFromXml(xml::XMLElement* e)
	{
	}
	void SetData(const core::string &name, const core::string &value, bool statusData)
	{
		std::vector<core::string> vals;
		vals = core::StringUtil::Split(value, ",");
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
			m_robotStatus.connected = true;
		}
		else if (name == "Disconnect" && vals.size() == 2)
		{
			m_robotStatus.connected = false;
		}

		m_robot->GetRobotController()-> UpdateRobotStatus(m_robotStatus);
	}
	void RemoveData(const core::string &key)
	{
	}
	void ClearData(bool statusValues)
	{
	}
};



LocalRobotCommunicator::LocalRobotCommunicator()
{
	m_impl = new LocalRobotCommunicatorImpl();
}
LocalRobotCommunicator::~LocalRobotCommunicator()
{
	delete m_impl;
}


bool LocalRobotCommunicator::Connect(const core::string& ip, int port)
{
	return m_impl->Connect(ip, port);
}
void LocalRobotCommunicator::Disconnect()
{
	m_impl->Disconnect();
}
bool LocalRobotCommunicator::IsConnected()
{
	return m_impl->IsConnected();
}

void LocalRobotCommunicator::SetUserID(const core::string& userID)
{
	m_impl->SetUserID(userID);
}
void LocalRobotCommunicator::ConnectUser(bool c)
{
	m_impl->ConnectUser(c);
}
void LocalRobotCommunicator::ConnectRobot(bool c)
{
	m_impl->ConnectRobot(c);
}

void LocalRobotCommunicator::Update(float dt)
{
	m_impl->Update(dt);

}

void LocalRobotCommunicator::LoadFromXml(xml::XMLElement* e)
{
	m_impl->LoadFromXml(e);
}


void LocalRobotCommunicator::SetData(const core::string &key, const core::string &value, bool statusData)
{
	m_impl->SetData(key, value, statusData);
}
void LocalRobotCommunicator::RemoveData(const core::string &key)
{
	m_impl->RemoveData(key);
}
void LocalRobotCommunicator::ClearData(bool statusValues)
{
	m_impl->ClearData(statusValues);
}

}
}

