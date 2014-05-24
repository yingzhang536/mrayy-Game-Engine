



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

#include "IMutex.h"

#include "AppData.h"
#include "DataTypes.h"

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
	class NodeRenderer;
	class SceneCamera;
class SessionRenderer :public ted::ISpeakerChangeListener
{
protected:
	NodeRenderer* m_nodeRenderer;
	ted::SessionContainer*m_sessions;

	std::list<ITedNode*> m_renderNodes;

	typedef std::map<ted::IDType, TweetNode*> TweetMap;
	typedef std::map<core::string, SpeakerNode*> SpeakerMap;


	float m_speakerDistance;
	float m_tweetsDistance;

	std::vector<SpeakerNode*> m_speakersSeq;
	SpeakerMap m_speakers;
	TweetMap m_tweets;

	msa::physics::World2D* m_physics;
	OS::IMutex* m_dataMutex;
	ITedNode* m_hoverItem;
	SceneCamera* m_camera;
	SpeakerNode* m_activeSpeaker;

	void _RenderConnections();
	void _RenderSpeakers();
	void _RenderTweets();

	math::rectf CalcAllBox();

public:
	SessionRenderer();
	virtual ~SessionRenderer();

	void SetRenderingVP(const math::rectf& vp);

	void SetSessions(ted::SessionContainer*sessions);

	virtual void _OnSpeakerChange(ted::CSpeaker* sp);

	void AddTweetsNodes(const std::vector<TweetNode*> &nodes);

	ITedNode* GetNodeFromPosition(const math::vector2d& pos);

	void SetHoverdItem(ITedNode* node);
	ITedNode* GetHoverdItem(){ return m_hoverItem; }

	void Draw();

	void Update(float dt);


};

}
}

#endif
