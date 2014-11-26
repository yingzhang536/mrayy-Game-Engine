

#ifndef IGStreamerStreamer_h__
#define IGStreamerStreamer_h__

#include "mString.h"

namespace mray
{
namespace video
{
	
class IGStreamerStreamer
{
protected:
public:
	IGStreamerStreamer(){}
	virtual ~IGStreamerStreamer(){}

	virtual void BindPorts(const core::string& addr, int port,bool rtcp) = 0;

	virtual bool CreateStream() = 0;
	virtual void Stream() = 0;
	virtual bool IsStreaming() = 0;
	virtual void Stop() = 0;
	virtual void Close()=0;

	virtual void SetPaused(bool paused) = 0;
	virtual bool IsPaused() = 0;
};

}
}

#endif // IGStreamerStreamer_h__
