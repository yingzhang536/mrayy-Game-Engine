

#include "stdafx.h"
#include "GstNetworkPlayer.h"

#include "ILogManager.h"

#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

#include "CMySrc.h"
#include "CMySink.h"

#include "VideoAppSinkHandler.h"
#include "GstPipelineHandler.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

namespace mray
{
namespace video
{

class GstNetworkPlayerImpl :public GstPipelineHandler
{
	core::string m_ipAddr;
	int m_videoPort;
	int m_audioPort;
	int m_rtcpPort;

	core::string m_pipeLineString;

	GstMyUDPSrc* m_videoSrc;
	GstMyUDPSrc* m_videoRtcpSrc;
	GstMyUDPSink* m_videoRtcpSink;

	GstMyUDPSrc* m_audioSrc;
	GstMyUDPSrc* m_audioRtcpSrc;
	GstMyUDPSink* m_audioRtcpSink;

	GstAppSink* m_videoSink;

	VideoAppSinkHandler m_videoHandler;

public:
	GstNetworkPlayerImpl()
	{
		m_ipAddr = "127.0.0.1";
		m_videoPort = 5000;
		m_audioPort = 5002;
		m_rtcpPort = 5004;
		m_pipeLineString =
		{
			"rtpbin "
				"name=rtpbin "

			//video rtp
			"myudpsrc "
				"name=videoSrc "
				"caps=application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264 "
			"! rtpbin.recv_rtp_sink_0 "
			"rtpbin. ! rtph264depay ! avdec_h264 "
			"! appsink name=videoSink "
			"myudpsrc name=videoRtcpSrc !"
			"rtpbin.recv_rtcp_sink_0 "

			//video rtcp
			"rtpbin.send_rtcp_src_0 ! "
			"myudpsink "
				"name=videoRtcpSink "
				"sync=false "
				"async=false "
			
			//audio rtp
			"myudpsrc "
				"name=audioSrc "
				"caps=application/x-rtp,media=(string)audio,clock-rate=(int)8000,encoding-name=(string)AMR,encoding-params=(string)1,octet-align=(string)1 "
			"! rtpbin.recv_rtp_sink_1 "
			"rtpbin. ! rtpamrdepay ! amrnbdec "
			"! directsoundsink "
			"myudpsrc name=audioRtcpSrc !"
			"rtpbin.recv_rtcp_sink_1 "

			//audio rtcp
			"rtpbin.send_rtcp_src_1 ! "
			"myudpsink "
				"name=audioRtcpSink "
				"sync=false "
				"async=false "

		};

	}
	virtual ~GstNetworkPlayerImpl()
	{

	}

	void SetIPAddress(const core::string& ip, int videoPort, int audioPort = 0, int rtcpPort = 0)
	{
		m_ipAddr = ip;
		m_videoPort = videoPort;
		m_audioPort = audioPort;
		m_rtcpPort = rtcpPort;

		if (!m_audioPort)
			m_audioPort = m_videoPort + 2;
		if (!m_rtcpPort)
			m_rtcpPort = m_audioPort + 2;
	}
	bool StartStream()
	{

		{
			core::string videoStr;
			videoStr = "myudpsrc name=videoSrc  "
				"caps=application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264 ! "
				" rtph264depay ! h264parse ! avdec_h264 ! "
				"videoconvert ! video/x-raw,format=RGB  !"
				"appsink name=videoSink ";

			m_pipeLineString = videoStr ;
		}

		GError *err = 0;
		m_gstPipeline = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			printf("GstNetworkPlayer: Pipeline error: %s", err->message);
		}
		if (!m_gstPipeline)
			return false;

#define SET_SRC(name,p) m_##name=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(m_gstPipeline), #name)); if(m_##name){m_##name->port=p;}
#define SET_SINK(name,p) m_##name=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(m_gstPipeline), #name)); if(m_##name){m_##name->host=m_ipAddr;m_##name->port=p;}

		SET_SRC(videoSrc, m_videoPort);
		SET_SINK(videoRtcpSink, (m_videoPort + 1));
		SET_SRC(videoRtcpSrc, m_rtcpPort);


		SET_SRC(audioSrc, m_audioPort);
		SET_SINK(audioRtcpSink, (m_audioPort + 1));
		SET_SRC(audioRtcpSrc, (m_rtcpPort + 1));

		m_videoSink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(m_gstPipeline), "videoSink"));

		m_videoHandler.SetSink(m_videoSink);

		//setup video sink
		gst_base_sink_set_sync(GST_BASE_SINK(m_videoSink), true);
		g_object_set(G_OBJECT(m_videoSink), "emit-signals", FALSE, "sync", false, (void*)NULL);

		//attach videosink callbacks
		GstAppSinkCallbacks gstCallbacks;
		gstCallbacks.eos = &VideoAppSinkHandler::on_eos_from_source;
		gstCallbacks.new_preroll = &VideoAppSinkHandler::on_new_preroll_from_source;
#if GST_VERSION_MAJOR==0
		gstCallbacks.new_buffer = &VideoAppSinkHandler::on_new_buffer_from_source;
#else
		gstCallbacks.new_sample = &VideoAppSinkHandler::on_new_buffer_from_source;
#endif
		gst_app_sink_set_callbacks(GST_APP_SINK(m_videoSink), &gstCallbacks, &m_videoHandler, NULL);


		return StartPipeline();

	}

	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		GstPipelineHandler::Close();
		m_videoHandler.Close();
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


	virtual const math::vector2di& GetFrameSize()
	{
		return m_videoHandler.GetFrameSize();
	}

	virtual video::EPixelFormat GetImageFormat()
	{
		return m_videoHandler.getPixelsRef()->format;
	}

	virtual bool GrabFrame(){ return m_videoHandler.GrabFrame(); }
	virtual bool HasNewFrame(){ return m_videoHandler.isFrameNew(); }
	virtual ulong GetBufferID(){ return m_videoHandler.GetFrameID(); }

	virtual const ImageInfo* GetLastFrame(){ return m_videoHandler.getPixelsRef(); }
};


GstNetworkPlayer::GstNetworkPlayer()
{
	m_impl = new GstNetworkPlayerImpl();
}

GstNetworkPlayer::~GstNetworkPlayer()
{
	delete m_impl;
}
void GstNetworkPlayer::SetIPAddress(const core::string& ip, int videoPort, int audioPort , int rtcpPort )
{
	m_impl->SetIPAddress(ip, videoPort, audioPort, rtcpPort);
}
bool GstNetworkPlayer::StartStream()
{
	return m_impl->StartStream();
}

void GstNetworkPlayer::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstNetworkPlayer::IsStream()
{
	return m_impl->IsStream();
}

void GstNetworkPlayer::Play()
{
	m_impl->Play();
}
void GstNetworkPlayer::Stop()
{
	m_impl->Stop();
}
void GstNetworkPlayer::Pause()
{
	m_impl->Pause();
}
bool GstNetworkPlayer::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstNetworkPlayer::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstNetworkPlayer::Close()
{
	m_impl->Close();

}

const math::vector2di& GstNetworkPlayer::GetFrameSize()
{
	return m_impl->GetFrameSize();
}

video::EPixelFormat GstNetworkPlayer::GetImageFormat()
{
	return m_impl->GetImageFormat();
}

bool GstNetworkPlayer::GrabFrame()
{
	return m_impl->GrabFrame();
}

bool GstNetworkPlayer::HasNewFrame()
{
	return m_impl->HasNewFrame();
}

ulong GstNetworkPlayer::GetBufferID()
{
	return m_impl->GetBufferID();
}


const ImageInfo* GstNetworkPlayer::GetLastFrame()
{
	return m_impl->GetLastFrame();
}


}
}


