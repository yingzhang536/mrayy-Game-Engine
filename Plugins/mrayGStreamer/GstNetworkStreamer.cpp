

#include "stdafx.h"
#include "GstNetworkStreamer.h"

#include "GstPipelineHandler.h"

#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"


namespace mray
{
namespace video
{

class GstNetworkStreamerImpl :public GstPipelineHandler
{
protected:
	core::string m_ipAddr;
	int m_videoPort;
	int m_audioPort;
	int m_rtcpPort;

	int m_cam0, m_cam1;
	int m_bitRate;
	bool m_streamAudio;

	math::vector2di m_frameSize;

	core::string m_pipeLineString;
	GstMyUDPSink* m_videoSink;
	GstMyUDPSink* m_videoRtcpSink;
	GstMyUDPSrc* m_videoRtcpSrc;

	GstMyUDPSink* m_audioSink;
	GstMyUDPSink* m_audioRtcpSink;
	GstMyUDPSrc* m_audioRtcpSrc;

public:
	GstNetworkStreamerImpl()
	{
		m_ipAddr = "127.0.0.1";
		m_videoPort = 5000;
		m_audioPort = 5002;
		m_rtcpPort = 5004;

		m_bitRate = 3000;
		m_cam0 = 0;
		m_cam1 = 1;
		m_streamAudio = true;

		m_frameSize.set(1280, 720);
	}

	virtual ~GstNetworkStreamerImpl()
	{
	}
	void BuildString()
	{
		core::string videoStr;
		if (m_cam0 < 0)m_cam0 = 0;
		if (m_cam1 < 0)m_cam1 = 0;
		if (m_cam0 == m_cam1)
		{
			//ksvideosrc
			videoStr = "ksvideosrc name=src device-index=" + core::StringConverter::toString(m_cam0) + // device=" + m_cam0.guidPath + "" +//
				" ! video/x-raw,format=I420,width=" + core::StringConverter::toString(m_frameSize.x) + ",height=" + core::StringConverter::toString(m_frameSize.y) + ",framerate=30/1 ! videoconvert !  videoflip method=4 ";  // videoflip method=1 !   ";

		}
		else
		{

			int halfW = m_frameSize.x / 2;
			videoStr = "videomixer name=mix sink_0::xpos=0   sink_0::ypos=0  sink_0::alpha=1  sink_0::zorder=0  sink_1::xpos=0   sink_1::ypos=0  sink_1::zorder=1     sink_2::xpos=" + core::StringConverter::toString(halfW) + "   sink_2::ypos=0  sink_2::zorder=1  ";

			videoStr += "videotestsrc pattern=\"black\" ! video/x-raw,format=I420,width=" + core::StringConverter::toString(m_frameSize.x) + ",height=" + core::StringConverter::toString(m_frameSize.y) + " !  mix.sink_0 ";

			//first camera
			videoStr += "ksvideosrc name=src1 device-index=" + core::StringConverter::toString(m_cam0) + "  ! video/x-raw,format=I420,width=" + core::StringConverter::toString(m_frameSize.x) + ",height=" + core::StringConverter::toString(m_frameSize.y) + ",framerate=30/1 ! videoconvert ! videoflip method=4 ! videoscale !"
				"video/x-raw,format=I420,width=" + core::StringConverter::toString(halfW) + ",height=" + core::StringConverter::toString(m_frameSize.y) + " ! mix.sink_1 ";
			
			//second camera
			videoStr += "ksvideosrc name=src2 device-index=" + core::StringConverter::toString(m_cam1) + "  ! video/x-raw,format=I420,width=" + core::StringConverter::toString(m_frameSize.x) + ",height=" + core::StringConverter::toString(m_frameSize.y) + ",framerate=30/1 ! videoconvert ! videoflip method=4 ! videoscale ! "
				"video/x-raw,format=I420,width=" + core::StringConverter::toString(halfW) + ",height=" + core::StringConverter::toString(m_frameSize.y) + "! mix.sink_2 ";

			videoStr += " mix. ";

		}
		m_pipeLineString = "rtpbin  name=rtpbin " + 
			videoStr + "! "
			//encoder string
			"x264enc name=videoEnc bitrate=" + core::StringConverter::toString(m_bitRate) + " speed-preset=superfast tune=zerolatency sync-lookahead=0 ! rtph264pay "
			"! rtpbin.send_rtp_sink_0 "
			"rtpbin.send_rtp_src_0 ! "
			"myudpsink name=videoSink "
			"rtpbin.send_rtcp_src_0 ! "
			"myudpsink name=videoRtcpSink sync=false async=false "
			"myudpsrc name=videoRtcpSrc ! rtpbin.recv_rtcp_sink_0 ";

		if (m_streamAudio)
		{
			m_pipeLineString+= " directsoundsrc! audio/x-raw,endianness=1234,signed=true,width=16,depth=16,rate=8000,channels=1   ! amrnbenc ! rtpamrpay !"
				"rtpbin.send_rtp_sink_1 "
				"rtpbin.send_rtp_src_1 ! myudpsink name=audioSink "
				"rtpbin.send_rtcp_src_1 ! "
				"myudpsink name=audioRtcpSink sync=false async=false "
				"myudpsrc name=audioRtcpSrc ! rtpbin.recv_rtcp_sink_1 ";
		}


		m_pipeLineString = videoStr + " ! "
			"x264enc name=videoEnc  speed-preset=superfast tune=zerolatency sync-lookahead=0 !  rtph264pay config-interval=1 !"
			"myudpsink name=videoSink ";


	}

	void SetBitRate(int bitRate)
	{
		m_bitRate = bitRate;
	}
	void SetCameras(int cam0, int cam1)
	{
		m_cam0 = 0;
		m_cam1 = 1;
	}
	void SetResolution(int width, int height)
	{
		m_frameSize.set(width, height);
	}

	void BindPorts(const core::string& addr, int videoPort, int audioPort = 0, int rtcpPort = 0)
	{
		m_ipAddr = addr;
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
		GError *err = 0;
		BuildString();
		m_gstPipeline = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			printf("GstNetworkPlayer: Pipeline error: %s", err->message);
		}
		if (!m_gstPipeline)
			return false;

#define SET_SRC(name,p) m_##name=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(m_gstPipeline), #name)); if(m_##name){m_##name->port=p;}
#define SET_SINK(name,p) m_##name=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(m_gstPipeline), #name)); if(m_##name){m_##name->host=m_ipAddr;m_##name->port=p;}

		SET_SINK(videoSink, m_videoPort);
		SET_SINK(videoRtcpSink, m_rtcpPort);
		SET_SRC(videoRtcpSrc, (m_videoPort + 1));


		SET_SINK(audioSink, m_audioPort);
		SET_SINK(audioRtcpSink, (m_rtcpPort + 1));
		SET_SRC(audioRtcpSrc, (m_audioPort + 1));


		return StartPipeline();

	}
	void Play()
	{
		SetPaused(false);
	}

	virtual void Close()
	{
		GstPipelineHandler::Close();
	}


};


GstNetworkStreamer::GstNetworkStreamer()
{
	m_impl = new GstNetworkStreamerImpl();
}

GstNetworkStreamer::~GstNetworkStreamer()
{
	delete m_impl;
}
void GstNetworkStreamer::Play()
{
	m_impl->Play();
}
void GstNetworkStreamer::Stop()
{
	m_impl->Stop();
}


void GstNetworkStreamer::BindPorts(const core::string& addr, int videoPort, int audioPort, int rtcpPort)
{
	m_impl->BindPorts(addr,videoPort, audioPort, rtcpPort);
}

bool GstNetworkStreamer::StartStream()
{
	return m_impl->StartStream();
}

void GstNetworkStreamer::Close()
{
	m_impl->Close();
}
void GstNetworkStreamer::SetBitRate(int bitRate)
{
	m_impl->SetBitRate(bitRate);
}


void GstNetworkStreamer::SetCameras(int cam0, int cam1)
{
	m_impl->SetCameras(cam0, cam1);
}

}
}

