
#include "stdafx.h"
#include "GGComponent.h"
#include "GGDriver.h"
#include "IFeedbackControl.h"

namespace mray
{
namespace VT
{

IMPLEMENT_PROPERTY_TYPE_HEADER(IP,GGComponent,core::string,mT("IP"),EPBT_Basic,mT(""),mT(""));
IMPLEMENT_PROPERTY_TYPE_GENERIC(IP,GGComponent,core::string,SetIP,GetIP,,,false);

IMPLEMENT_PROPERTY_TYPE_HEADER(Port,GGComponent,int,mT("Port"),EPBT_Basic,mT(""),0);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Port,GGComponent,int,SetPort,GetPort,core::StringConverter::toString,core::StringConverter::toInt,false);



class GGComponentImpl
{
public:
	GGComponentImpl()
	{
		port=1234;
		driver=0;
	}
	~GGComponentImpl()
	{
		if(driver)
		{
			driver->Close();
			delete driver;
		}
	}
	core::string ip;
	int port;

	IForceFeedbackDriver* driver;

	GGFeedbackValue feedback;

};

GGComponent::GGComponent(game::GameEntityManager* m)
{
	m_impl=new GGComponentImpl();
	m_impl->driver=new GGDriver();

	CPropertieDictionary* dic;
	if(CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeIP::instance);
		dic->addPropertie(&PropertyTypePort::instance);
	}
}
GGComponent::~GGComponent()
{
	delete m_impl;
}
bool GGComponent::InitComponent()
{
	if(!IVTComponent::InitComponent())
		return false;
	m_impl->driver->Init(m_impl->ip,m_impl->port);

	return true;
}

bool GGComponent::SetIP(const core::string& ip)
{
	m_impl->ip=ip;
	return true;
}
const core::string& GGComponent::GetIP()
{
	return m_impl->ip;
}

bool GGComponent::SetPort(int p)
{
	m_impl->port=p;
	return true;
}
int GGComponent::GetPort()
{
	return m_impl->port;
}

void GGComponent::SetChannelValue(int c,float v)
{
	if(c>=16)
		return;
	m_impl->feedback.channels[c]=v;
}

void GGComponent::Update(float dt)
{
	m_impl->driver->SendData(&m_impl->feedback);
}



}
}

