

#ifndef GstNetworkStreamer_h__
#define GstNetworkStreamer_h__


#include "IGStreamerPlayer.h"

namespace mray
{
namespace video
{

class GstNetworkStreamerImpl;

class GstNetworkStreamer 
{
protected:
	GstNetworkStreamerImpl* m_impl;
public:
	GstNetworkStreamer();
	virtual ~GstNetworkStreamer();

	// addr: target address to stream video to
	// videoport: port for the video stream, video rtcp is allocated as videoPort+1
	// audioport: port for the audio stream (0 --> videoPort+2) , audio rtcp is allocated as audioPort+1
	// rtcpPort: port for realtime control protocol stream, 0 --> audioPort+2
	void BindPorts( const core::string& addr, int videoPort, int audioPort = 0, int rtcpPort = 0);
	void SetResolution(int width, int height);
	void SetBitRate(int bitRate);

	bool StartStream();
	void Play();
	void Stop();
	virtual void Close();

	void SetCameras(int cam0, int cam1);

};

}
}

#endif // GstNetworkStreamer_h__
