

#include "stdafx.h"
#include "DataCommunicator.h"

#include "INetwork.h"
#include "IThreadFunction.h"
#include "IThreadManager.h"
#include "CMemoryStream.h"
#include "ATAppGlobal.h"
#include "GeomDepthRect.h"
#include "OpenNIHandler.h"
#include "INetwork.h"

namespace mray
{
namespace AugTel
{


	enum class EMessages
	{
		DepthData = 1,
		DepthSize = 2
	};
	class DataCommunicatorThread :public OS::IThreadFunction
	{
		DataCommunicator* m_owner;
	public:
		DataCommunicatorThread(DataCommunicator* o)
		{
			m_owner = o;
		}
		virtual void execute(OS::IThread*caller, void*arg)
		{
			while (caller->isActive())
			{
				if (m_owner->_Process() != 0)
				{
					return;
				}
			}
		}

	};
DataCommunicator::DataCommunicator()
{
	m_client = network::INetwork::getInstance().createUDPClient();
}
DataCommunicator::~DataCommunicator()
{
	delete m_client;
}

void DataCommunicator::Start(int port)
{
	m_client->Open(port);
	m_thread = OS::IThreadManager::getInstance().createThread(new DataCommunicatorThread(this));
	m_thread->start(0);
}

void DataCommunicator::Stop()
{
	m_client->Close();
	OS::IThreadManager::getInstance().killThread(m_thread);
	delete m_thread;
	m_thread = 0;
}

int DataCommunicator::_Process()
{

	if (!m_client->IsOpen())
		return -1;
#define MAX_BUFFER 65536
	char buffer[MAX_BUFFER];
	network::NetAddress src;
	uint len = MAX_BUFFER;
	if (m_client->RecvFrom(buffer, &len, &src,0) != network::UDP_SOCKET_ERROR_NONE)
		return 0;//failed to receive the packet
	OS::CMemoryStream stream("", (uchar*)buffer, len, false);
	int msg = 0;
	stream.read(&msg,sizeof(msg));

	switch (msg)
	{
	case (int)EMessages::DepthData:
	{
		TBee::GeomDepthRect rc;
		rc.ReadFromStream(&stream);
		ATAppGlobal::Instance()->depthProvider->GetNormalCalculator().AddDepthRect(&rc);
	}break;
	case (int)EMessages::DepthSize:
	{
		math::vector2di sz;
		stream.read(&sz,sizeof(sz));
		ATAppGlobal::Instance()->depthProvider->CreateDepthFrame(sz.x, sz.y);
	}break;
	default:
		break;
	}

	return 0;
}

}
}


