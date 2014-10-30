

#ifndef GstStreamBin_h__
#define GstStreamBin_h__


#include "IGStreamerStreamer.h"
#include "GCPtr.h"
#include "mstring.h"
#include <map>

namespace mray
{
namespace video
{

class GstStreamBin
{
protected:
	typedef std::map<core::string, GCPtr<IGStreamerStreamer>> StreamMap;
	StreamMap m_Streamers;
public:
	GstStreamBin();
	virtual ~GstStreamBin();

	void AddStream(IGStreamerStreamer* Streamer, const core::string& name);
	IGStreamerStreamer* GetStream(const core::string& name);

	void Stream();
	void Stop();
	void CloseAll();

	void StartStream(const core::string& name);
	void StopStream(const core::string& name);
	IGStreamerStreamer* RemoveStream(const core::string& name, bool close);
	void ClearStreams(bool stop);
};

}
}


#endif // GstStreamBin_h__
