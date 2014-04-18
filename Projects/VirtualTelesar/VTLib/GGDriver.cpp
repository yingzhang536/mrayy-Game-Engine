

#include "stdafx.h"
#include "GGDriver.h"

#include "IFeedbackControl.h"
#include "mywsock.h"


namespace mray
{
namespace VT
{


class GGDriverImpl
{
protected:
	UDPSender* m_sender;
	int m_port;
	core::string m_ipAddress;
public:
	GGDriverImpl()
	{
		m_sender=0;
		m_port=-1;
	}
	virtual~GGDriverImpl()
	{
	}

	void Init(const core::string& ip,int port)
	{
		m_ipAddress=ip;
		m_port=port;
		m_sender=new UDPSender(m_port,m_ipAddress.c_str());
		m_sender->startSending();
	}
	void Close()
	{
		if(m_sender)
			m_sender->stopSending();
	}
	void SendData(IFeedbackResult* res)
	{
		if(!m_sender)
			return;
		core::string sendBuff;
		GGFeedbackValue* v=dynamic_cast<GGFeedbackValue*>(res);
		if(v)
		{
			const int sendMlt=1000;
			int vals[18];
			for (int i = 0; i<GGFeedbackValue::ChannelsCount; ++i)
			{
				vals[i]= v->channels[i]*sendMlt;

				sendBuff += core::StringConverter::toString(vals[i])+" ";
			}
			//convert data 
			//sprintf(sendBuff,"%5d %5d %5d %5d %5d %5d ",vals[0],vals[1],vals[2],vals[3],vals[4],vals[5]);
			m_sender->setBuffer(sendBuff);
		}
	}

};


GGDriver::GGDriver()
{
	m_impl=new GGDriverImpl();
}
GGDriver::~GGDriver()
{
	delete m_impl;
}

void GGDriver::Init(const core::string& ip,int port)
{
	m_impl->Init(ip,port);
}
void GGDriver::Close()
{
	m_impl->Close();
}
void GGDriver::SendData(IFeedbackResult* res)
{
	m_impl->SendData(res);
}

}
}
