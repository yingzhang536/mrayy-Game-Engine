
#include "stdafx.h"
#include "CRobotConnector.h"
#include "RemoteRobotCommunicator.h"
#include "TBAppGlobals.h"
#include "OculusDevice.h"
#include "INetwork.h"
#include "JoystickDefinitions.h"

namespace mray
{
namespace TBee
{
CRobotConnector::CRobotConnector()
{
	m_connected = false;
	m_status = false;
	m_communicator = new TBee::RemoteRobotCommunicator();

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
	m_communicator->ClearData();
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
	m_communicator->SetData("Connect", addrStr);
}
void CRobotConnector::ConnectRobotIP(const core::string& ip, int videoport, int audioPort)
{
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
	m_communicator->SetData("Disconnect", addrStr);
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

void CRobotConnector::HandleController()
{
	controllers::IJoysticController* joystick = TBee::TBAppGlobals::inputMngr->getJoystick(0);
	if (!joystick)
	{
		m_speed = 0;
		m_rotation = 0;
		return;
	}
	controllers::JoysticAxis x = joystick->getAxisState(JOYSTICK_Axis0);
	controllers::JoysticAxis y = joystick->getAxisState(JOYSTICK_Axis1);
	controllers::JoysticAxis r = joystick->getAxisState(JOYSTICK_Axis3);

	m_speed.x = x.abs;
	m_speed.y = y.abs;
	m_rotation = r.abs;
}
void CRobotConnector::UpdateStatus()
{
	if (!m_status || !m_connected)
		return;
	HandleController();
	if (TBee::TBAppGlobals::oculusDevice)
		TBee::TBAppGlobals::oculusDevice->GetOrientation().toEulerAngles(m_head);
	m_communicator->SetData("Head", core::StringConverter::toString(m_head));
	m_communicator->SetData("Speed", core::StringConverter::toString(m_speed));
	m_communicator->SetData("Rotation", core::StringConverter::toString(m_rotation));
}


}
}