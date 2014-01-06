

/********************************************************************
	created:	2013/12/04
	created:	4:12:2013   12:43
	filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\RobotCommunicator.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
	file base:	RobotCommunicator
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __RobotCommunicator__
#define __RobotCommunicator__


#include "IUDPClient.h"

#include "TelubeeRobotDLL.h"

namespace mray
{



	struct UserStatus
	{
		network::NetAddress address;
	};


	class RobotCommunicator;
	class IRobotCommunicatorListener
	{
	public:

		virtual void OnUserDisconnected(RobotCommunicator* sender, const network::NetAddress& address){}
		virtual void OnUserConnected(RobotCommunicator* sender, const network::NetAddress& address, int videoPort, int audioPort){};
		virtual void OnRobotStatus(RobotCommunicator* sender, const RobotStatus& status){};

		virtual void OnCollisionData(RobotCommunicator* sender, float left, float right){}
	};


class RobotCommunicator:public ITelubeeRobotListener
{
protected:

	CTelubeeRobotDLL* m_robotController;
	RobotStatus m_robotStatus;
	UserStatus m_userStatus;
	void HandleData(const core::string& name, const core::string& value);
	void ProcessPacket(const char* buffer);

	network::IUDPClient* m_client;

	OS::IThread* m_thread;

	IRobotCommunicatorListener* m_listener;

	bool m_localControl;

	void _RobotStatus(const RobotStatus& st);
public:
	RobotCommunicator();
	virtual~RobotCommunicator();

	void StartServer(int port);
	void StopServer();

	void SetLocalControl(bool c){ m_localControl = c; }
	bool IsLocalControl(){ return m_localControl; }

	void SetRobotData(const RobotStatus &st);

	void SetListener(IRobotCommunicatorListener* l){ m_listener = l; }

	int _Process();

	virtual void OnCollisionData(float left, float right);

};

}


#endif
