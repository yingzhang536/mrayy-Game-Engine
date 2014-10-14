

#ifndef GstNetworkPlayer_h__
#define GstNetworkPlayer_h__


#include "IGStreamerPlayer.h"

namespace mray
{
namespace video
{

	class GstNetworkPlayerImpl;

class GstNetworkPlayer :public IGStreamerPlayer
{
protected:
	GstNetworkPlayerImpl* m_impl;
public:
	GstNetworkPlayer();
	virtual ~GstNetworkPlayer();

	//set ip address for the target host
	// videoport: port for the video stream, video rtcp is allocated as videoPort+1
	// audioport: port for the audio stream (0 --> videoPort+2) , audio rtcp is allocated as audioPort+1
	// rtcpPort: port for realtime control protocol stream, 0 --> audioPort+2
	void SetIPAddress(const core::string& ip, int videoPort, int audioPort = 0, int rtcpPort = 0);
	bool StartStream();

	virtual bool IsStream() ;

	void SetVolume(float vol);

	virtual void Play() ;
	virtual void Pause();
	virtual void Stop();
	virtual bool IsLoaded() ;
	virtual bool IsPlaying();
	virtual void Close() ;

	//defined by the source video stream
	virtual void SetFrameSize(int w, int h) {}
	virtual const math::vector2di& GetFrameSize() ;

	//defined by the source video stream
	virtual void SetImageFormat(video::EPixelFormat fmt){}
	virtual video::EPixelFormat GetImageFormat() ;

	virtual bool GrabFrame() ;
	virtual bool HasNewFrame() ;
	virtual ulong GetBufferID() ;// incremented once per frame

	virtual const ImageInfo* GetLastFrame() ;

};

}
}

#endif // GstNetworkPlayer_h__
