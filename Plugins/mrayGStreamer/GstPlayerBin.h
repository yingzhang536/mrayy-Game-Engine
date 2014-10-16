

#ifndef GstPlayerBin_h__
#define GstPlayerBin_h__

#include "IGStreamerPlayer.h"
#include "GCPtr.h"
#include "mstring.h"

namespace mray
{
namespace video
{
	
class GstPlayerBin
{
protected:
	typedef std::map<core::string, GCPtr<IGStreamerPlayer>> PlayerMap;
	PlayerMap m_players;
public:
	GstPlayerBin();
	virtual ~GstPlayerBin();

	void AddPlayer(IGStreamerPlayer* player, const core::string& name);
	IGStreamerPlayer* GetPlayer(const core::string& name);

	void Play();
	void Stop();
	void CloseAll();

	void StartPlayer(const core::string& name);
	void StopPlayer(const core::string& name);
	IGStreamerPlayer* RemovePlayer(const core::string& name, bool close);
	void ClearPlayers(bool stop);
};

}
}

#endif // GstPlayerBin_h__
