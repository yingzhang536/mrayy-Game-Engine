

/********************************************************************
	created:	2014/04/10
	created:	10:4:2014   16:28
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\TweetParticle.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	TweetParticle
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __TweetParticle__
#define __TweetParticle__

#include "IBaseParticle.h"


namespace mray
{
	namespace ted
	{
		class TweetDB;
	}
namespace scene
{

	class TweetsEmitter;

class TweetParticle:public IBaseParticle
{
protected:
	ted::TweetDB* m_tweet;
	float m_targetRadius;
public:
	TweetParticle(TweetsEmitter* e,ted::TweetDB* tweet);
	virtual~TweetParticle();

	ted::TweetDB* GetTweet(){ return m_tweet; }

	void SetTargetRadius(float r)
	{
		m_targetRadius = r;
	}

	virtual void update(float dt);

};

}
}


#endif