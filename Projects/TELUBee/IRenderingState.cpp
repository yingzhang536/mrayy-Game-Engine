

#include "StdAfx.h"
#include "IRenderingState.h"

#include "Application.h"
#include "TBAppGlobals.h"


namespace mray
{
namespace TBee
{

	void IRenderingState::LoadFromXML(xml::XMLElement* e)
	{
		xml::XMLAttribute* a=e->getAttribute("Name");
		if(a)
			m_name=a->value;
	}

	void IRenderingState::InitState(Application* app)
	{
		gLogManager.log("Initing Rendering State", ELL_INFO);

		int c=1;
		if(TBAppGlobals::StereoMode!=scene::EStereo_None)
			c=2;
		for(int i=0;i<2;++i)
		{
			m_rtTexture[i]=app->getDevice()->createEmptyTexture2D(true);
			m_rtTexture[i]->setMipmapsFilter(false);

			m_renderTarget[i]=app->getDevice()->createRenderTarget("",m_rtTexture[i],0,0,0);
		}
	}

	video::IRenderTarget* IRenderingState::Render(const math::rectf& rc,ETargetEye eye)
	{
		int index=GetEyeIndex(eye);
		//math::vector2d ratio=TBAppGlobals::GetStereoScaleRatio();
		if(m_rtTexture[index]->getSize().x!=rc.getWidth() ||
			m_rtTexture[index]->getSize().y!=rc.getHeight())
		{
			m_rtTexture[index]->createTexture(math::vector3d(rc.getWidth(),rc.getHeight(),1),video::EPixel_R8G8B8);
		}
		return m_renderTarget[index];

	}

}
}

