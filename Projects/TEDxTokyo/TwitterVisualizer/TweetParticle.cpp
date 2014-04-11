

#include "stdafx.h"
#include "TweetParticle.h"
#include "TweetDB.h"
#include "TweetsEmitter.h"



namespace mray
{
namespace ted
{
	class TweetDB;
}
namespace scene
{

TweetParticle::TweetParticle(TweetsEmitter* e, ted::TweetDB* tweet)
:IBaseParticle(e), m_tweet(tweet)
{
}
TweetParticle::~TweetParticle()
{

}

void TweetParticle::update(float dt)
{
	math::vector3d parent= m_emitter->GetSystem()->getOwner()->getAbsolutePosition();
	math::vector3d dir = position - parent;
	float len = dir.Normalize();
	float diff = len-m_targetRadius;
	if (diff <= 0)
	{
		position = parent+dir*m_targetRadius;
		velocity = 0;
	}
	else
	{
		velocity = -dir*10;
	}
	IBaseParticle::update(dt);
}

}
}
