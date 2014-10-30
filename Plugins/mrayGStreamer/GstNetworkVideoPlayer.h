

#ifndef GstNetworkVideoPlayer_h__
#define GstNetworkVideoPlayer_h__


#include "IGStreamerPlayer.h"
#include "IVideoGrabber.h"

namespace mray
{
namespace video
{

class GstNetworkVideoPlayerImpl;

class GstNetworkVideoPlayer :public IGStreamerPlayer, public IVideoGrabber
{
protected:
	GstNetworkVideoPlayerImpl* m_impl;
public:
	GstNetworkVideoPlayer();
	virtual ~GstNetworkVideoPlayer();

	//set ip address for the target host
	// videoport: port for the video stream, video rtcp is allocated as videoPort+1 and videoPort+2
	void SetIPAddress(const core::string& ip, int videoPort,bool rtcp);
	bool CreateStream();

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

#endif // GstNetworkVideoPlayer_h__
