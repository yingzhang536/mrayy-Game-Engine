


#ifndef GstNetworkAudioStreamer_h__
#define GstNetworkAudioStreamer_h__

#include "IGStreamerStreamer.h"

namespace mray
{
namespace video
{

	class GstNetworkAudioStreamerImpl;

	
class GstNetworkAudioStreamer:public IGStreamerStreamer
{
protected:
	GstNetworkAudioStreamerImpl* m_impl;
public:
	GstNetworkAudioStreamer();
	virtual ~GstNetworkAudioStreamer();

	// addr: target address to stream video to
	// audioport: port for the audio stream , audio rtcp is allocated as audioPort+1 and audioPort+2
	void BindPorts(const core::string& addr, int audioPort );

	bool CreateStream();
	void Stream();
	bool IsStreaming();
	void Stop();
	virtual void Close();

};

}
}

#endif // GstNetworkAudioStreamer_h__
