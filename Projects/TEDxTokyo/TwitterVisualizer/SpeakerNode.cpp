

#include "stdafx.h"
#include "SpeakerNode.h"
#include "CSpeaker.h"
#include "ShaderResourceManager.h"

#include "TweetNode.h"
#include "NodeRenderer.h"
#include "TwitterUserProfile.h"

namespace mray
{
namespace scene
{

SpeakerNode::SpeakerNode(ted::CSpeaker* s)
{
	m_speaker = s;
}
SpeakerNode::~SpeakerNode()
{

}
ted::IDType SpeakerNode::GetUserID()
{

	if (!m_speaker->GetUser())
		return 0;
	return m_speaker->GetUser()->ID;
}
core::string SpeakerNode::GetUserDisplyName()
{
	return m_speaker->GetTwitterID();
}

void SpeakerNode::AddTweet(TweetNode* t)
{
	m_subTweets.push_back(t);
	t->SetParent(this);
}
void SpeakerNode::Draw(NodeRenderer *r)
{
	r->AddSpeaker(this);
	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		r->AddSpeakerTweetNode(this, m_subTweets[i]);
	}

	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		m_subTweets[i]->Draw(r);
	}
}
void SpeakerNode::Update(float dt)
{
	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		m_subTweets[i]->Update(dt);
	}
}
ITedNode* SpeakerNode::GetNodeFromPoint(const math::vector2d& pos)
{
	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		ITedNode* n= m_subTweets[i]->GetNodeFromPoint(pos);
		if (n)
			return n;
	}
	return ITedNode::GetNodeFromPoint(pos);
}

math::rectf SpeakerNode::GetBoundingBox(bool includeChildren)
{
	math::rectf rc = ITedNode::GetBoundingBox(false);
	if (!includeChildren)
		return rc;
	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		math::rectf r = m_subTweets[i]->GetBoundingBox(true);
		rc.addPoint(r.ULPoint);
		rc.addPoint(r.BRPoint);
	}
	return rc;
}
}
}

