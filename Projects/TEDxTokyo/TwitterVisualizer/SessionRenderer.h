



/********************************************************************
	created:	2014/05/07
	created:	7:5:2014   17:30
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\SessionRenderer.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	SessionRenderer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __SessionRenderer__
#define __SessionRenderer__




namespace mray
{
	namespace ted
	{
		class SessionContainer;
		class CSpeaker;
	}
namespace scene
{
	class ITedNode;
	class SpeakerNode;
	class TweetNode;
class SessionRenderer
{
protected:
	ted::SessionContainer*m_sessions;

	std::list<ITedNode*> m_renderNodes;

	std::vector<SpeakerNode*> m_speakers;
	std::vector<TweetNode*> m_tweets;
	msa::physics::World2D* m_physics;
public:
	SessionRenderer();
	virtual ~SessionRenderer();

	void SetSessions(ted::SessionContainer*sessions);

	void _OnSpeakerChanged(ted::CSpeaker*s);

	void Draw();

	void Update(float dt);
};

}
}

#endif
