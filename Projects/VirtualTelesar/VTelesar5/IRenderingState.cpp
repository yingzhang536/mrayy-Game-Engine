

#include "StdAfx.h"
#include "IRenderingState.h"
#include "Application.h"


namespace mray
{
namespace VT
{

	void IRenderingState::LoadFromXML(xml::XMLElement* e)
	{
		xml::XMLAttribute* a=e->getAttribute("Name");
		if(a)
			m_name=a->value;
	}

	void IRenderingState::InitState(Application* app)
	{

		m_stereo=app->IsStereo();
		int c=m_stereo?2:1;
		for(int i=0;i<c;++i)
		{
			m_rtTexture[i]=app->getDevice()->createEmptyTexture2D(true);
			m_rtTexture[i]->setMipmapsFilter(false);

			m_renderTarget[i]=app->getDevice()->createRenderTarget("",m_rtTexture[i],0,0,0);
		}
	}
	video::IRenderTarget* IRenderingState::Render(bool left,const math::rectf& rc)
	{
		int index=left?0:1;
		if(!m_stereo)
			index=0;
		if(m_rtTexture[index]->getSize().x!=rc.getWidth() ||
			m_rtTexture[index]->getSize().y!=rc.getHeight())
		{
			m_rtTexture[index]->createTexture(math::vector3d(rc.getWidth(),rc.getHeight(),1),video::EPixel_R8G8B8);
		}
		return m_renderTarget[index];
	}
	video::IRenderTarget* IRenderingState::GetLastFrame(bool left)
	{
		int index=left?0:1;
		if(!m_stereo)
			index=0;
		return m_renderTarget[index].pointer();
	}
}
}

