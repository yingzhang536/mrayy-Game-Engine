

#include "stdafx.h"
#include "GStreamVideoProvider.h"

#include "ofVideoPlayerGst.h"


namespace mray
{
namespace TBee
{

GStreamVideoProvider::GStreamVideoProvider()
{
	m_playBack=new ofVideoPlayerGst();
	//m_playBack->loadMovie("C:\\fingers.mov");
	//m_playBack->play();
	m_playBack->Connect("192.168.10.170",170,5000);

}
GStreamVideoProvider::~GStreamVideoProvider()
{
	delete m_playBack;
}

void GStreamVideoProvider::ConnectToCameras(const core::string& ip,int port1,int port2)
{
	m_playBack->Connect(ip,port1,port2);
}
void GStreamVideoProvider::Disconnect()
{
	m_playBack->close();
}
bool GStreamVideoProvider::IsConnected()
{
	return m_playBack->IsConnected();
}

bool GStreamVideoProvider::HasNewImage(int id)
{
	return m_playBack->isFrameNew();
}
const video::ImageInfo* GStreamVideoProvider::GetImage(int id)
{
	return m_playBack->getTextureReference();
}
math::rectf	GStreamVideoProvider::GetTexRect(int id)
{
	static const math::rectf trcs[]=
	{
		math::rectf(0,0,0.5,1),
		math::rectf(0.5,0,1,1)
	};
	return trcs[id];
}

void GStreamVideoProvider::Update(float dt)
{
	m_playBack->update();
}


}
}

