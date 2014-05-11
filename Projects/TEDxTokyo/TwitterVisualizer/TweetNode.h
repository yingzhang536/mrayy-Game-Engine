
#ifndef TweetNode_h__
#define TweetNode_h__

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
public:
	TweetNode(ted::CSpeaker* s,ted::TwitterTweet* t);
	virtual~TweetNode();

	ted::CSpeaker* GetSpeaker(){ return m_targetSpeaker; }
	ted::TwitterTweet* GetTweet(){ return m_tweet; }

	void AddTweet(TweetNode* t)
	{
		m_subTweets.push_back(t);
	}
	void Draw();
};

}
}

#endif // TweetNode_h__
