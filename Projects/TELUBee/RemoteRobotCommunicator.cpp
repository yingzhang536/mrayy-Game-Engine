
#include "stdafx.h"
#include "RemoteRobotCommunicator.h"
#include "TBeeOptiTrackObject.h"
#include "shmem.h"
#include "multicast.h"
#include "IUDPClient.h"
#include "INetwork.h"
#include "IThreadFunction.h"
#include "IThread.h"
#include "IThreadManager.h"
#include "multicast.h"
#include "TBAppGlobals.h"
#include "TBRobotInfo.h"

namespace mray
{
namespace TBee
{
#define USE_UDPCLIENT
#define USE_OPTI
class RemoteRobotCommunicatorData;
class RemoteRobotCommunicatorThread :public OS::IThreadFunction
{
	RemoteRobotCommunicatorData* m_owner;
public:
	RemoteRobotCommunicatorThread(RemoteRobotCommunicatorData* o)
	{
		m_owner = o;
	}
	virtual void execute(OS::IThread*caller, void*arg);
};
class RemoteRobotCommunicatorData
{
public:
	struct shmem_data data;
	RemoteRobotCommunicator* owner;
#ifdef USE_UDPCLIENT
	network::IUDPClient* client;
#else
	UDPSender*	client;
#endif
	network::NetAddress addr;
	OS::IThread* thread;
	bool connected;


	std::map<core::string, core::string> values;

	core::string outputValues;

	RemoteRobotCommunicatorData(RemoteRobotCommunicator* o)
	{
		owner = o;
		connected = false;
#ifdef USE_UDPCLIENT
		client = network::INetwork::getInstance().createUDPClient();
		client->Open();
#else
		client = new UDPSender();
		client->startSending();
#endif
		memset(&data, 0, sizeof(shmem_data_t));
	}
	~RemoteRobotCommunicatorData()
	{
		if (client)
		{
#ifdef USE_UDPCLIENT
			client->Close();
#else
			client->stopSending();
#endif
			delete client;
			client = 0;
		}
	}
	int getmyip(){
		char ac[80];
		if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
			printf("Error when getting local host name \n");
			return 1;
		}

		struct hostent *phe = gethostbyname(ac);
		if (phe == 0) {
			printf("Bad host lookup.\n");
			return 1;
		}

		for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
			data.user.ipaddress = addr;
		}

		return 0;
	}

	void UpdateData()
	{

		xml::XMLWriter w;
		xml::XMLElement root("RobotData");
		root.addAttribute("Connected", core::StringConverter::toString(data.robot.status.connected));
		if (data.robot.status.connected)
		{
			for (std::map<core::string, core::string>::iterator it = values.begin(); it != values.end(); ++it)
			{
				xml::XMLElement *e = new xml::XMLElement("Data");
				e->addAttribute("N", it->first);
				e->addAttribute("V", it->second);
				root.addSubElement(e);
			}
		}
		w.addElement(&root);
		outputValues = w.flush();
	}


	void SetData(const core::string &key, const core::string &v)
	{

		values[key] = v;
		UpdateData();
	}
};


void RemoteRobotCommunicatorThread::execute(OS::IThread*caller, void*arg)
{
	while (caller->isActive())
	{
		if (m_owner->connected)
		{
			m_owner->owner->Update(0);
		}
		if (m_owner->data.robot.status.connected && m_owner->connected)
		{
			OS::IThreadManager::getInstance().sleep(2);
		}
		else
		{
			OS::IThreadManager::getInstance().sleep(100);
		}
	}
}
RemoteRobotCommunicator::RemoteRobotCommunicator()
{
	m_data = new RemoteRobotCommunicatorData(this);
	m_data->data.user.status.online = true;
	m_data->getmyip();
	//	Connect("127.0.0.1",3000);

	m_data->thread = OS::IThreadManager::getInstance().createThread(new RemoteRobotCommunicatorThread(m_data));
	m_data->thread->start(0);
}
RemoteRobotCommunicator::~RemoteRobotCommunicator()
{
	OS::IThreadManager::getInstance().killThread(m_data->thread);
	delete m_data;
}

bool RemoteRobotCommunicator::Connect(const core::string& ip, int port)
{
	if (m_data->connected)
		Disconnect();
	bool res = false;
	m_data->addr = network::NetAddress(ip, port);
#ifdef USE_UDPCLIENT
	if (m_data->client->Connect(m_data->addr))
#else
	if (m_data->client->Connect(port, ip.c_str()))
#endif
	{

#ifdef USE_OPTI
		{
			core::string ipaddr = inet_ntoa(m_data->data.user.ipaddress);
			//	TBAppGlobals::optiObj->Connect(ipaddr,ipaddr);
		}
#endif
		m_data->connected = true;
		res = true;
	}

	return res;
}
void RemoteRobotCommunicator::Disconnect()
{
	if (!m_data->connected)
		return;
#ifdef USE_OPTI
	//TBAppGlobals::optiObj->Disconnect();
#endif
#ifdef USE_UDPCLIENT
	m_data->client->Disconnect();
#else
	m_data->client->stopSending();
#endif
	m_data->connected = false;
}
bool RemoteRobotCommunicator::IsConnected()
{
	return m_data->connected;
}

void RemoteRobotCommunicator::SetUserID(const core::string& userID)
{
	m_data->data.user.user_id = userID.c_str();
}

void RemoteRobotCommunicator::ConnectUser(bool c)
{
	m_data->data.user.status.connected = c;
	_SendUpdate();
}
void RemoteRobotCommunicator::ConnectRobot(bool c)
{
	m_data->data.robot.status.connected = c;
	_SendUpdate();
}

void RemoteRobotCommunicator::SetData(const core::string &key, const core::string &value)
{
	m_data->SetData(key, value);
}

void RemoteRobotCommunicator::Update(float dt)
{
	//return ;
	if (!m_data->connected)
		return;

	//construct the xml data fields


#ifdef USE_UDPCLIENT
	m_data->client->SendTo(&m_data->addr, m_data->outputValues.c_str(), m_data->outputValues.length() + 1);
#else
	m_data->client->setBuffer(str.c_str());
#endif
}

void RemoteRobotCommunicator::_SendUpdate()
{/*
	core::string ipaddr;
	if (TBee::TBAppGlobals::selectedRobot)
		ipaddr = TBee::TBAppGlobals::selectedRobot->IP;//inet_ntoa(m_data->data.user.ipaddress);
	else
		return;*/
	char buffer[512];
	//sprintf(buffer, "%s %d %d", ipaddr.c_str(), m_data->data.user.status.connected, m_data->data.robot.status.connected);

	m_data->UpdateData();
	m_data->client->SendTo(&m_data->addr, m_data->outputValues.c_str(), m_data->outputValues.length() + 1);

	return;
#ifdef USE_UDPCLIENT
	m_data->client->SendTo(&m_data->addr, buffer, strlen(buffer));
#else
	m_data->client->setBuffer(buffer);
#endif


}
void RemoteRobotCommunicator::LoadFromXml(xml::XMLElement* e)
{
}

}
}

