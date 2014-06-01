

#include "stdafx.h"
#include "TweetNode.h"
#include "TwitterTweet.h"
#include "TwitterUserProfile.h"
#include "InternetCacheManager.h"
#include "TextureResourceManager.h"
#include "ShaderResourceManager.h"

#include "NodeRenderer.h"
#include "CSpeaker.h"

namespace mray
{
namespace scene
{

TweetNode::TweetNode(ted::CSpeaker* speaker, ted::TwitterTweet* t)
{
	m_hoverValue = 0;
	m_targetSpeaker = speaker;
	m_tweet = t;
	video::ITexturePtr tex = gTextureResourceManager.getResource(m_tweet->user->imageUrl);
	if (tex.isNull())
	{
		OS::IStreamPtr s = network::InternetCacheManager::getInstance().GetOrCreateItem(m_tweet->user->imageUrl);
		if (s)
		{
			tex = gTextureResourceManager.loadTexture2D(m_tweet->user->imageUrl, s);
			tex->load(false);
		}
	}
	m_texture.SetTexture(tex);

	m_alpha = 0;
	m_sizeScaler = 0.5;

}
TweetNode::~TweetNode()
{

}

ted::IDType TweetNode::GetTweetID()
{
	return m_tweet->ID;
}
ted::IDType TweetNode::GetSpeakerID()
{
	return m_targetSpeaker->GetUser()->ID;
}
void TweetNode::Update(float dt)
{
	const float speed = 1;
#define DECREASE(x,l,v) if(x>(l)){ x-=(v);} if(x<(l)){x=(l);}
#define INCREASE(x,l,v) if(x<(l)){ x+=(v);} if(x>(l)){x=(l);}

	if (IsHovered(true))
	{
		INCREASE(m_hoverValue, 1,speed*dt);
	}
	else
	{
		DECREASE(m_hoverValue, 0,speed*dt);

	}
	if (IsHovered(false))
	{
		INCREASE(m_sizeScaler, 1, speed*dt);
	}
	else
	{
		DECREASE(m_sizeScaler, 0.5, speed*dt);
	}
	
	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		m_subTweets[i]->Update(dt);
	}

	if (m_alpha < 1)
	{
		m_alpha +=0.1* dt;
		if (m_alpha > 1)
			m_alpha = 1;
	}
}
void TweetNode::Draw(NodeRenderer *r, const math::rectf& rc)
{
	if (rc.IsRectCollide(GetBoundingBox(false)))
		r->AddTweet(this, m_sizeScaler);

	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		r->AddTweetTweet(this, m_subTweets[i]);
	}

	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		m_subTweets[i]->Draw(r,rc);
	}
}

ITedNode* TweetNode::GetNodeFromPoint(const math::vector2d& pos)
{
	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		ITedNode* n = m_subTweets[i]->GetNodeFromPoint(pos);
		if (n)
			return n;
	}
	math::rectf rc;
	rc.ULPoint = GetPosition() - m_sizeScaler*GetSize()*0.5;
	rc.BRPoint = GetPosition() + m_sizeScaler*GetSize()*0.5;
	if (rc.IsPointInside(pos))
		return this;
	return 0;
}
math::rectf TweetNode::GetBoundingBox(bool includeChildren)
{
	math::rectf rc = ITedNode::GetBoundingBox(false);
	if (!includeChildren)
		return rc;
	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		math::rectf r= m_subTweets[i]->GetBoundingBox(true);
		rc.addPoint(r.ULPoint);
		rc.addPoint(r.BRPoint);
	}
	return rc;
}

}
}
