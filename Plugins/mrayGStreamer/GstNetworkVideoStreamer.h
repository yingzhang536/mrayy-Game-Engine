

#ifndef GstNetworkVideoStreamer_h__
#define GstNetworkVideoStreamer_h__

#include "IGStreamerStreamer.h"

namespace mray
{
namespace video
{

class GstNetworkVideoStreamerImpl;

class GstNetworkVideoStreamer :public IGStreamerStreamer
{
protected:
	GstNetworkVideoStreamerImpl* m_impl;
public:
	GstNetworkVideoStreamer();
	virtual ~GstNetworkVideoStreamer();

	// addr: target address to stream video to
	// videoport: port for the video stream, video rtcp is allocated as videoPort+1/videoPort+2
	void BindPorts( const core::string& addr, int videoPort);
	void SetResolution(int width, int height);
	void SetBitRate(int bitRate);


	void EnableVideo(bool v);
	void EnableAudio(bool v);
	bool IsAudioEnabled();
	bool IsVideoEnabled();

	bool CreateStream();
	void Stream();
	bool IsStreaming();
	void Stop();
	virtual void Close();

	void SetCameras(int cam0, int cam1);
	bool IsStereo();

};

}
}

#endif // GstNetworkVideoStreamer_h__
