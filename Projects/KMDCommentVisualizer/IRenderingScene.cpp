
#include "stdafx.h"
#include "IRenderingScene.h"

#include "Engine.h"


namespace mray
{
namespace kmd
{


void IRenderingScene::Init()
{

	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	m_rtTexture = dev->createEmptyTexture2D(true);
	m_rtTexture->setMipmapsFilter(false);

	m_renderTarget = dev->createRenderTarget("", m_rtTexture, 0, 0, 0);

}

video::IRenderTarget* IRenderingScene::Draw(const math::rectf& rc)
{

	if (m_rtTexture->getSize().x != rc.getWidth() ||
		m_rtTexture->getSize().y != rc.getHeight())
	{
		m_rtTexture->createTexture(math::vector3d(rc.getWidth(), rc.getHeight(), 1), video::EPixel_R8G8B8);
	}
	return m_renderTarget;
}

}
}

