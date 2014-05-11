

#include "stdafx.h"
#include "TweetNode.h"
#include "TwitterTweet.h"
#include "TwitterUserProfile.h"
#include "InternetCacheManager.h"
#include "TextureResourceManager.h"
#include "ShaderResourceManager.h"


namespace mray
{
namespace scene
{

TweetNode::TweetNode(ted::CSpeaker* speaker, ted::TwitterTweet* t)
{
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

	video::IGPUShaderProgram *shader = (video::IGPUShaderProgram*)gShaderResourceManager.getResource("ProfileMasking").pointer();
	if (shader)
	{
		dev->setFragmentShader(shader);
		float a = 1;
		shader->setConstant("Alpha", &a, 1);
	}
	
	dev->useTexture(0, &m_texture);
	dev->draw2DImage(math::rectf(pos - m_sz*0.5f, pos + m_sz*0.5f), 1);

	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		m_subTweets[i]->Draw();
	}
}

}
}
