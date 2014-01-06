

#include "stdafx.h"
#include "GStreamVideoProvider.h"

#include "ofGstVideoPlayer.h"


namespace mray
{
namespace TBee
{

GStreamVideoProvider::GStreamVideoProvider()
{
	m_playBack=new ofGstVideoPlayer();
	//m_playBack->Connect("192.168.10.170",170,5000);

}
GStreamVideoProvider::~GStreamVideoProvider()
{
	delete m_playBack;
}

void GStreamVideoProvider::LoadMovie(const core::string& path)
{
	m_playBack->loadMovie(path);
	m_playBack->play();

}

void GStreamVideoProvider::ConnectToCameras(const core::string& ip, int  videoPort, int audioPort, int localPort)
{
	m_playBack->Connect(ip,videoPort,audioPort,localPort);
	m_playBack->play();
}
void GStreamVideoProvider::Disconnect()
{
	m_playBack->close();
}
bool GStreamVideoProvider::IsConnected()
{
	return m_playBack->isLoaded();
}

bool GStreamVideoProvider::HasNewImage(int id)
{
	return m_playBack->isFrameNew();
}
const video::ImageInfo* GStreamVideoProvider::GetImage(int id)
{
	return m_playBack->getPixelsRef();
}
math::rectf	GStreamVideoProvider::GetTexRect(int id)
{
	static const math::rectf trcs[]=
	{
		math::rectf(0,0,1,1),
		math::rectf(0,0,1,1)
	};
	return trcs[id];
}

void GStreamVideoProvider::Update(float dt)
{
	m_playBack->update();
}


}
}

