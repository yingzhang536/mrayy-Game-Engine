
#include "stdafx.h"
#include "HMDRobotRenderer.h"
#include "UDPVideoProvider.h"
#include "IRenderArea.h"
#include "Application.h"
#include "GStreamVideoProvider.h"

namespace mray
{
namespace TBee
{

HMDRobotRenderer::HMDRobotRenderer()
{
	m_provider=new GStreamVideoProvider(); //UDPVideoProvider();//
}

HMDRobotRenderer::~HMDRobotRenderer()
{
	delete m_provider;
}

void HMDRobotRenderer::Init(Application*app)
{
	m_remoteTex[0]=app->getDevice()->createEmptyTexture2D(true);
	m_remoteTex[1]=app->getDevice()->createEmptyTexture2D(true);
}
void HMDRobotRenderer::Grab()
{
	for(int i=0;i<2;++i)
	{
		if(m_provider->HasNewImage(i))
		{
			const video::ImageInfo* image=m_provider->GetImage(i);
			m_remoteTex[i]->createTexture(math::vector3d(image->Size.x,image->Size.y,1),image->format);

			video::LockedPixelBox box(math::box3d(0,image->Size),image->format,image->imageData);
			m_remoteTex[i]->getSurface(0)->blitFromMemory(box);
		}
	}
}
void HMDRobotRenderer::Render(const math::rectf &rc,ETargetEye eye)
{
	math::rectf trc;
	if(eye==Eye_Left)
	{
		m_tUnit.SetTexture(m_remoteTex[0]);
		trc=m_provider->GetTexRect(0);
	}
	else
	{
		m_tUnit.SetTexture(m_remoteTex[1]);
		trc=m_provider->GetTexRect(1);
	}
	Engine::getInstance().getDevice()->useTexture(0,&m_tUnit);;
	Engine::getInstance().getDevice()->draw2DImage(rc,1,0,&trc);
}

void HMDRobotRenderer::Update(float dt)
{
	m_provider->Update(dt);
}

void HMDRobotRenderer::Enter()
{
}

void HMDRobotRenderer::Exit()
{
}

void HMDRobotRenderer::LoadFromXML(xml::XMLElement* e)
{
}


}
}
