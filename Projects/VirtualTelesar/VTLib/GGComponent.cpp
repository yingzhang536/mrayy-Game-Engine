
#include "stdafx.h"
#include "GGComponent.h"
#include "GGDriver.h"
#include "IFeedbackControl.h"
#include "FontResourceManager.h"
#include "IGUIRenderer.h"

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
	if (c >= GGFeedbackValue::ChannelsCount)
		return;
	if (v > 0)
	{
		printf("");
	}
	m_impl->feedback.channels[c]=v;
}

void GGComponent::Update(float dt)
{
	m_impl->driver->SendData(&m_impl->feedback);
}

void GGComponent::OnGUIRender(GUI::IGUIRenderer* renderer, const math::rectf& vp)
{
	IVTComponent::OnGUIRender(renderer, vp);
	return;
	GUI::IFont* font = gFontResourceManager.getDefaultFont();
	GUI::FontAttributes attr;
	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	if (font)
	{
		attr.fontColor.Set(1, 0, 0, 1);
		attr.fontAligment = GUI::EFA_TopLeft;
		attr.fontSize = 18;
		attr.hasShadow = true;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;

		math::rectf r = vp;
		r.ULPoint.y += 200;

		core::string msg = mT("Channels: ") + m_impl->feedback.ToString();
		font->print(r, &attr, 0, msg, renderer);
		r.ULPoint.y += attr.fontSize + 5;
	}
}


}
}

