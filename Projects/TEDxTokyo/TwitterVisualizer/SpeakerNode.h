

#ifndef SpeakerNode_h__
#define SpeakerNode_h__

#include "DataTypes.h"
#include "ITedNode.h"

namespace mray
{
	namespace ted
	{
		class CSpeaker;
	}
namespace scene
{
	class TweetNode;

class SpeakerNode :public ITedNode
{
protected:
	ted::CSpeaker* m_speaker;
	std::vector<TweetNode*> m_subTweets;
	float m_glowFactor;
public:
	SpeakerNode(ted::CSpeaker* s);
	virtual~SpeakerNode();

	ted::CSpeaker* GetSpeaker(){
		return m_speaker;
	}

	ted::IDType GetUserID();
	core::string GetUserDisplyName();

	void AddTweet(TweetNode* t);
	const std::vector<TweetNode*>& GetSubTweets(){ return m_subTweets; }
	virtual void Update(float dt);


	void Draw(NodeRenderer *r, const math::rectf& rc);
	void DrawSubTweets();
	virtual ITedNode* GetNodeFromPoint(const math::vector2d& pos);
	virtual math::rectf GetBoundingBox(bool includeChildren);
};

}
}

#endif // SpeakerNode_h__
