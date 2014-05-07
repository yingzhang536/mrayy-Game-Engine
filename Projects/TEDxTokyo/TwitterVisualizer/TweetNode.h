
#ifndef TweetNode_h__
#define TweetNode_h__

namespace mray
{
	namespace ted
	{
		class TwitterTweet;
	}

namespace scene
{

class TweetNode
{
protected:
	ted::TwitterTweet* m_tweet;
public:
	TweetNode();
	virtual~TweetNode();
};

}
}

#endif // TweetNode_h__
