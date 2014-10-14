

#include "stdafx.h"
#include "GStreamerNetworkPlayer.h"




namespace mray
{
namespace video
{

class GStreamerNetworkPlayerImpl 
{
	code::string m_ipAddr;
	int m_videoPort;
	int m_audioPort;
	int m_rtcpPort;
public:
	GStreamerNetworkPlayerImpl()
	{

	}
	virtual ~GStreamerNetworkPlayerImpl()
	{

	}

	void SetIPAddress(const core::string& ip, int videoPort, int audioPort = 0, int rtcpPort = 0)
	{

	}

	bool IsStream()
	{
		return true;
	}

	void Play()
	{

	}
	void Pause()
	{

	}
	bool IsLoaded()
	{

	}
	bool IsPlaying()
	{

	}
	void Close()
	{

	}

};


GStreamerNetworkPlayer::GStreamerNetworkPlayer()
{
	m_impl = new GStreamerNetworkPlayerImpl();
}

GStreamerNetworkPlayer::~GStreamerNetworkPlayer()
{
	delete m_impl;
}
void GStreamerNetworkPlayer::SetIPAddress(const core::string& ip, int videoPort, int audioPort , int rtcpPort )
{
	m_impl->SetIPAddress(ip, videoPort, audioPort, rtcpPort);
}

bool GStreamerNetworkPlayer::IsStream()
{
	return m_impl->IsStream();
}

void GStreamerNetworkPlayer::Play()
{
	m_impl->Play();
}
void GStreamerNetworkPlayer::Pause()
{
	m_impl->Pause();
}
bool GStreamerNetworkPlayer::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GStreamerNetworkPlayer::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GStreamerNetworkPlayer::Close()
{
	m_impl->Close();

}

}
}


