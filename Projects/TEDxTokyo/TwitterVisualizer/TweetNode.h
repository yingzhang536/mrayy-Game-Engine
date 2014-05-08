
#ifndef TweetNode_h__
#define TweetNode_h__

#include "ITedNode.h"

namespace mray
{
	namespace ted
	{
		class TwitterTweet;
	}

namespace scene
{

class TweetNode:public ITedNode
{
protected:
	ted::CSpeaker* m_targetSpeaker;
	ted::TwitterTweet* m_tweet;
	msa::physics::Particle2D* m_phNode;
public:
	TweetNode();
	virtual~TweetNode();
};

}
}

#endif // TweetNode_h__
