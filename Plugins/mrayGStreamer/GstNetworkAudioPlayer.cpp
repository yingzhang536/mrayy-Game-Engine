

#include "stdafx.h"
#include "GstNetworkAudioPlayer.h"

#include "ILogManager.h"

#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

#include "CMySrc.h"
#include "CMySink.h"
#include "GstPipelineHandler.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

namespace mray
{
namespace video
{

class GstNetworkAudioPlayerImpl :public GstPipelineHandler
{
	core::string m_ipAddr;
	int m_audioPort;

	core::string m_pipeLineString;

	GstMyUDPSrc* m_audioSrc;
	GstMyUDPSrc* m_audioRtcpSrc;
	GstMyUDPSink* m_audioRtcpSink;
	bool m_rtcp;

public:
	GstNetworkAudioPlayerImpl()
	{
		m_ipAddr = "127.0.0.1";
		m_audioPort = 5005;
		m_gstPipeline = 0;
	}
	virtual ~GstNetworkAudioPlayerImpl()
	{

	}


	void _BuildPipeline()
	{
		core::string audioStr =
			"myudpsrc name=audioSrc "
			"caps=application/x-rtp,media=(string)audio,clock-rate=(int)8000,encoding-name=(string)AMR,encoding-params=(string)1,octet-align=(string)1 ";
		if (m_rtcp)
		{
			m_pipeLineString =
				"rtpbin name=rtpbin "
				+ audioStr +
				//audio rtp
				"! rtpbin.recv_rtp_sink_0 "

				"rtpbin. ! rtpamrdepay ! amrnbdec ! directsoundsink "

				"myudpsrc name=audioRtcpSrc ! rtpbin.recv_rtcp_sink_0 "

				//audio rtcp
				"rtpbin.send_rtcp_src_0 ! myudpsink name=audioRtcpSink sync=false async=false ";
		}
		else
		{
			m_pipeLineString = audioStr + " ! "
				"rtpbin. ! rtpamrdepay ! amrnbdec ! directsoundsink ";

		}

	}
	void _UpdatePorts()
	{
		if (!m_gstPipeline)
			return;
#define SET_SRC(name,p) m_##name=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(m_gstPipeline), #name)); if(m_##name){m_##name->SetPort(p);}
#define SET_SINK(name,p) m_##name=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(m_gstPipeline), #name)); if(m_##name){m_##name->SetPort(m_ipAddr,p);}


		SET_SRC(audioSrc, m_audioPort);
		SET_SINK(audioRtcpSink, (m_audioPort + 1));
		SET_SRC(audioRtcpSrc, (m_audioPort + 2));

	}

	void SetIPAddress(const core::string& ip, int audioPort, bool rtcp)
	{
		m_ipAddr = ip;
		m_audioPort = audioPort;
		m_rtcp = rtcp;

		//set src and sinks elements
		_UpdatePorts();
	}
	bool CreateStream()
	{
		if (m_Loaded)
			return true;

		_BuildPipeline();

		GError *err = 0;
		m_gstPipeline = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			printf("GstNetworkAudioPlayer: Pipeline error: %s", err->message);
		}
		if (!m_gstPipeline)
			return false;

		_UpdatePorts();

		return CreatePipeline();

	}

	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		GstPipelineHandler::Close();
	}

	void Play()
	{
		SetPaused(false);
	}

	void Pause()
	{
		SetPaused(true);
	}
	void SetVolume(float vol)
	{
		g_object_set(G_OBJECT(m_gstPipeline), "volume", (gdouble)vol, (void*)0);
	}


};


GstNetworkAudioPlayer::GstNetworkAudioPlayer()
{
	m_impl = new GstNetworkAudioPlayerImpl();
}

GstNetworkAudioPlayer::~GstNetworkAudioPlayer()
{
	delete m_impl;
}
void GstNetworkAudioPlayer::SetIPAddress(const core::string& ip, int audioPort,bool rtcp)
{
	m_impl->SetIPAddress(ip, audioPort,rtcp);
}
bool GstNetworkAudioPlayer::CreateStream()
{
	return m_impl->CreateStream();
}

void GstNetworkAudioPlayer::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstNetworkAudioPlayer::IsStream()
{
	return m_impl->IsStream();
}

void GstNetworkAudioPlayer::Play()
{
	m_impl->Play();
}
void GstNetworkAudioPlayer::Stop()
{
	m_impl->Stop();
}
void GstNetworkAudioPlayer::Pause()
{
	m_impl->Pause();
}
bool GstNetworkAudioPlayer::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstNetworkAudioPlayer::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstNetworkAudioPlayer::Close()
{
	m_impl->Close();

}

}
}


