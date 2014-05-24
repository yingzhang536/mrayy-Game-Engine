
#ifndef TweetNode_h__
#define TweetNode_h__

#include "DataTypes.h"
#include "ITedNode.h"

namespace mray
{
	namespace ted
	{
		class TwitterTweet;
		class CSpeaker;
	}

namespace scene
{

class TweetNode:public ITedNode
{
protected:
	ted::CSpeaker* m_targetSpeaker;
	ted::TwitterTweet* m_tweet;
	video::TextureUnit m_texture;
	std::vector<TweetNode*> m_subTweets;

	float m_hoverValue;
public:
	TweetNode(ted::CSpeaker* s,ted::TwitterTweet* t);
	virtual~TweetNode();

	ted::IDType GetTweetID();
	ted::IDType GetSpeakerID();

	ted::CSpeaker* GetSpeaker(){ return m_targetSpeaker; }
	ted::TwitterTweet* GetTweet(){ return m_tweet; }

	video::TextureUnit& GetImage(){ return m_texture; }

	float GetHoverValue(){ return m_hoverValue; }

	void AddTweet(TweetNode* t)
	{
		m_subTweets.push_back(t);
		t->SetParent(this);
	}
	const std::vector<TweetNode*>& GetSubTweets(){ return m_subTweets; }
	virtual void Update(float dt);

	void Draw(NodeRenderer *r);
	virtual ITedNode* GetNodeFromPoint(const math::vector2d& pos);
	virtual math::rectf GetBoundingBox(bool includeChildren);
	virtual void OnHoverOn(){ m_hovered = true; }
	virtual void OnHoverOff(){ m_hovered = false; }
};

}
}

#endif // TweetNode_h__
