

#ifndef SpeakerNode_h__
#define SpeakerNode_h__

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
public:
	SpeakerNode(ted::CSpeaker* s);
	virtual~SpeakerNode();

	void AddTweet(TweetNode* t)
	{
		m_subTweets.push_back(t);
	}

	void Draw();
};

}
}

#endif // SpeakerNode_h__
