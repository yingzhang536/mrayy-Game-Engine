

#include "stdafx.h"
#include "SpeakerNode.h"
#include "CSpeaker.h"


namespace mray
{
namespace scene
{

SpeakerNode::SpeakerNode(ted::CSpeaker* s)
{
	m_speaker = s;
	m_phNode = 0;
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
	
	video::TextureUnit tex;
	tex.SetTexture(m_speaker->GetTexture());
	dev->useTexture(0, &tex);
	dev->draw2DImage(math::rectf(pos - 20, pos + 20), 1);
}

}
}

