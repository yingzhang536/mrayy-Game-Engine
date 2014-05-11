

#include "stdafx.h"
#include "SpeakerNode.h"
#include "CSpeaker.h"
#include "ShaderResourceManager.h"

#include "TweetNode.h"

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


void SpeakerNode::Draw()
{
	math::vector2d pos;
	
	if (m_phNode)
		pos=m_phNode->getPosition();

	video::IVideoDevice* dev= Engine::getInstance().getDevice();

	video::IGPUShaderProgram *shader = (video::IGPUShaderProgram*)gShaderResourceManager.getResource("ProfileMasking").pointer();
	if (shader)
	{
		dev->setFragmentShader(shader);
		float a = 1;
		shader->setConstant("Alpha", &a, 1);
	}
	video::TextureUnit tex;
	tex.SetTexture(m_speaker->GetTexture());
	dev->useTexture(0, &tex);
	dev->draw2DImage(math::rectf(pos - m_sz*0.5f, pos + m_sz*0.5f), 1);

	for (int i = 0; i < m_subTweets.size(); ++i)
	{
		m_subTweets[i]->Draw();
	}
}

}
}

