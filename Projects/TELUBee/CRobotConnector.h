

/********************************************************************
	created:	2013/12/04
	created:	4:12:2013   20:23
	filename: 	C:\Development\mrayEngine\Projects\TELUBee\CRobotConnector.h
	file path:	C:\Development\mrayEngine\Projects\TELUBee
	file base:	CRobotConnector
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __CRobotConnector__
#define __CRobotConnector__




namespace mray
{
	namespace TBee
	{
		class RemoteRobotCommunicator;

class CRobotConnector
{
protected:
	core::string m_robotIP;
	TBee::RemoteRobotCommunicator* m_communicator;
	bool m_connected;
	bool m_status;

	math::vector2d m_speed;
	math::vector3d m_head;
	float m_rotation;

	int m_videoPort;
	int m_audioPort;
public:
	CRobotConnector();
	~CRobotConnector();
	bool IsRobotConnected();
	void ConnectRobot();
	void ConnectRobotIP(const core::string& ip,int videport,int audioPort);
	void DisconnectRobot();
	void StartUpdate();
	void EndUpdate();
	void LoadXML(xml::XMLElement* e);

	void HandleController();
	void UpdateStatus();

};
	}

}


#endif
