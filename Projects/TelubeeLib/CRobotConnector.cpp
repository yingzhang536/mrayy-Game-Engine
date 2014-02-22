
#include "stdafx.h"
#include "CRobotConnector.h"
#include "RemoteRobotCommunicator.h"
#include "INetwork.h"
#include "AppData.h"
#include "IHeadController.h"
#include "IInputController.h"

namespace mray
{
namespace TBee
{
CRobotConnector::CRobotConnector()
{
	m_connected = false;
	m_status = false;
	m_communicator = new TBee::RemoteRobotCommunicator();
	m_headController = 0;
	m_robotController = 0;

	m_videoPort = -1;
	m_audioPort = -1;
}
CRobotConnector::~CRobotConnector()
{
	DisconnectRobot();
	delete m_communicator;
}
bool CRobotConnector::IsRobotConnected()
{
	return m_status;
}
void CRobotConnector::ConnectRobot()
{
	if (m_connected)
		m_communicator->Disconnect();
	m_connected = m_communicator->Connect(m_robotIP, 6000);
	m_communicator->ClearData(true);
	//	m_roboComm->Connect("127.0.0.1",3000);
	m_communicator->SetUserID("yamens");
	m_communicator->ConnectUser(true);
	network::NetAddress addr;
	char ipLst[8][16];
	network::INetwork::getInstance().getLocalIPs(ipLst);
	addr.port = m_videoPort;
	addr.setIP(ipLst[0]);
	core::string addrStr = addr.toString();
	addrStr += "," + core::StringConverter::toString(m_videoPort);
	addrStr += "," + core::StringConverter::toString(m_audioPort);
	m_communicator->SetData("Connect", addrStr,true);

	addrStr =  core::StringConverter::toString(m_commPort);
	m_communicator->SetData("CommPort", addrStr, true);

}
void CRobotConnector::ConnectRobotIP(const core::string& ip, int videoport, int audioPort,int commPort)
{
	m_commPort = commPort;
	m_videoPort = videoport;
	m_audioPort = audioPort;
	m_robotIP = ip;
	ConnectRobot();
}


void CRobotConnector::DisconnectRobot()
{
	if (!m_connected)
		return;
	network::NetAddress addr;
	char ipLst[8][16];
	network::INetwork::getInstance().getLocalIPs(ipLst);
	addr.port = m_videoPort;
	addr.setIP(ipLst[0]);
	core::string addrStr = addr.toString();
	addrStr += "," + core::StringConverter::toString(m_videoPort);
	m_communicator->SetData("Disconnect", addrStr, true);
	m_communicator->Update(0);//only once
	EndUpdate();
	m_communicator->Disconnect();
	m_connected = false;
}
void CRobotConnector::StartUpdate()
{
	if (!m_connected)
		return;
	m_status = true;

	m_communicator->ConnectRobot(true);
}
void CRobotConnector::EndUpdate()
{
	m_status = false;
	m_communicator->ConnectRobot(false);
}
void CRobotConnector::LoadXML(xml::XMLElement* e)
{

	xml::XMLAttribute*attr = e->getAttribute("IP");
	if (attr)
	{
		m_robotIP = attr->value;
	}
}
void CRobotConnector::SetData(const core::string& key, const core::string& val,bool status)
{
	m_communicator->SetData(key, val,status);
}
void CRobotConnector::RemoveData(const core::string& key)
{
	m_communicator->RemoveData(key);

}

void CRobotConnector::HandleController()
{
	if (!m_robotController)
	{
		m_speed = 0;
		m_rotation = 0;
		return;
	}
	m_speed = m_robotController->GetSpeed();
	m_rotation = m_robotController->GetRotation();;
}
void CRobotConnector::UpdateStatus()
{
	if (!m_status || !m_connected)
		return;
	HandleController();
	if (m_headController)
		m_headController->GetHeadOrientation().toEulerAngles(m_head);
	m_communicator->SetData("Head", core::StringConverter::toString(m_head),false);
	m_communicator->SetData("Speed", core::StringConverter::toString(m_speed), false);
	m_communicator->SetData("Rotation", core::StringConverter::toString(m_rotation), false);
}


}
}