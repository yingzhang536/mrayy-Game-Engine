

/********************************************************************
	created:	2014/02/22
	created:	22:2:2014   17:54
	filename: 	C:\Development\mrayEngine\Plugins\mrayGStreamer\IGStreamerPlayer.h
	file path:	C:\Development\mrayEngine\Plugins\mrayGStreamer
	file base:	IGStreamerPlayer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __IGStreamerPlayer__
#define __IGStreamerPlayer__

#include "IVideoGrabber.h"


namespace mray
{
namespace video
{

class IGStreamerPlayer:public IVideoGrabber
{
protected:
	bool m_isStreaming;
	virtual void _SetPipeline(const core::string& pipeline, bool stream);
public:
	IGStreamerPlayer(){}
	virtual~IGStreamerPlayer(){}

	virtual bool IsStream() = 0;

	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual bool IsLoaded() = 0;
	virtual bool IsPlaying() = 0;
	virtual void Close() = 0;

};

}
}


#endif
