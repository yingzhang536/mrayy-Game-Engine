

#include "stdafx.h"
#include "GstStreamerVideoSource.h"
#include "VideoGrabberTexture.h"

#include "GstNetworkVideoPlayer.h"
#include "GstNetworkAudioPlayer.h"




namespace mray
{
namespace TBee
{

GstStreamerVideoSource::GstStreamerVideoSource(const core::string& ip, int videoport, int audioport)
{
	m_player = new video::GstPlayerBin();

	video::GstNetworkVideoPlayer*vp = new video::GstNetworkVideoPlayer();
	video::GstNetworkAudioPlayer*ap = new video::GstNetworkAudioPlayer();

	m_player->AddPlayer(vp, "Video");
	m_player->AddPlayer(ap, "Audio");


	m_playerGrabber = new video::VideoGrabberTexture();
	SetIP(ip, videoport,audioport);
	m_isStereo = true;
}

GstStreamerVideoSource::~GstStreamerVideoSource()
{
	m_player->ClearPlayers(true);
	delete m_playerGrabber;
}

void GstStreamerVideoSource::Init()
{
	m_playerGrabber->Set(((video::GstNetworkVideoPlayer*)m_player->GetPlayer("Video")), 0);
}
void GstStreamerVideoSource::Open()
{
	((video::GstNetworkVideoPlayer*)m_player->GetPlayer("Video"))->SetIPAddress(m_ip, m_vport);
	((video::GstNetworkAudioPlayer*)m_player->GetPlayer("Audio"))->SetIPAddress(m_ip, m_aport);

	((video::GstNetworkVideoPlayer*)m_player->GetPlayer("Video"))->CreateStream();
	((video::GstNetworkAudioPlayer*)m_player->GetPlayer("Audio"))->CreateStream();
	m_player->Play();
}
void GstStreamerVideoSource::Close()
{
	m_player->CloseAll();

}
bool GstStreamerVideoSource::Blit(int eye)
{
	return m_playerGrabber->Blit();
}

math::vector2d GstStreamerVideoSource::GetEyeScalingFactor(int i)
{
	return math::vector2d(m_isStereo ? 1 : 1, 1);
}
math::vector2d GstStreamerVideoSource::GetEyeResolution(int i)
{
	math::vector3di sz = m_playerGrabber->GetTexture()->getSize();
	return math::vector2d(sz.x / (m_isStereo ? 2 : 1), sz.y);
}
video::ITexturePtr GstStreamerVideoSource::GetEyeTexture(int i)
{
	return m_playerGrabber->GetTexture();
}
math::rectf GstStreamerVideoSource::GetEyeTexCoords(int i)
{
	if (m_isStereo)
		return math::rectf(i*0.5, 0, 0.5 + i*0.5, 1);
	else
		return math::rectf(0, 0, 1, 1);
}

void GstStreamerVideoSource::SetIsStereo(bool stereo)
{
	m_isStereo = stereo;
}


}
}


