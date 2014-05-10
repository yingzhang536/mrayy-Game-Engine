

#include "stdafx.h"
#include "TweetNode.h"
#include "TwitterTweet.h"
#include "TwitterUserProfile.h"
#include "InternetCacheManager.h"
#include "TextureResourceManager.h"


namespace mray
{
namespace scene
{

TweetNode::TweetNode(ted::CSpeaker* speaker, ted::TwitterTweet* t)
{
	m_targetSpeaker = speaker;
	m_tweet = t;
	OS::IStreamPtr s= network::InternetCacheManager::getInstance().GetOrCreateItem(m_tweet->user->imageUrl);
	if (s)
	{
		video::ITexturePtr tex = gTextureResourceManager.loadTexture2D(m_tweet->user->imageUrl, s);
		tex->load(false);
		m_texture.SetTexture(tex);
	}
}
TweetNode::~TweetNode()
{

}


void TweetNode::Draw()
{
	math::vector2d pos;

	if (m_phNode)
		pos = m_phNode->getPosition();

	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	
	dev->useTexture(0, &m_texture);
	dev->draw2DImage(math::rectf(pos - 20, pos + 20), 1);
}

}
}
