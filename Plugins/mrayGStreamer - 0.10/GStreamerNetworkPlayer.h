

#ifndef GStreamerNetworkPlayer_h__
#define GStreamerNetworkPlayer_h__


#include "IGStreamerPlayer.h"

namespace mray
{
namespace video
{

	class GStreamerNetworkPlayerImpl;

class GStreamerNetworkPlayer :public IGStreamerPlayer
{
protected:
	GStreamerNetworkPlayerImpl* m_impl;
public:
	GStreamerNetworkPlayer();
	virtual ~GStreamerNetworkPlayer();

	//set ip address for the target host
	// video port: port for the video stream
	// audio port: port for the audio stream, 0 --> videoPort+1
	// rtcpPort: port for realtime control protocol stream, 0 --> videoport+2
	void SetIPAddress(const core::string& ip, int videoPort, int audioPort = 0, int rtcpPort = 0);

	virtual bool IsStream() ;

	virtual void Play() ;
	virtual void Pause() ;
	virtual bool IsLoaded() ;
	virtual bool IsPlaying();
	virtual void Close() ;
	
};

}
}

#endif // GStreamerNetworkPlayer_h__
