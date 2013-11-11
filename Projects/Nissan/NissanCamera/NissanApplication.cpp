

#include "stdafx.h"
#include "NissanApplication.h"


namespace mray
{


NissanApplication::NissanApplication()
{
	m_switcher=false;
}

NissanApplication::~NissanApplication()
{
}


void NissanApplication::onEvent(Event* event)
{
}


void NissanApplication::init(const OptionContainer &extraOptions)
{
	CMRayApplication::init(extraOptions);
	bool usbCamera=true;
	{

		core::string v=extraOptions.GetOptionValue("USBCamera");
		if(v=="Yes")
			usbCamera=true;
		else
			usbCamera=false;
	}
	m_viewPort=GetRenderWindow()->CreateViewport(mT("Main"),0,0,math::rectf(0,0,1,1),0);
	m_viewPort->SetClearColor(video::SColor(0,0,1,0));
	m_viewPort->setAutoClearRT(true);

	m_cameras[0]=new CameraRenderingState();
	m_cameras[1]=new CameraRenderingState();
	m_cameras[0]->SetCameraInfo(0,640,480,30,usbCamera);
	m_cameras[1]->SetCameraInfo(1,640,480,30,usbCamera);
	m_cameras[0]->InitCamera();
	m_cameras[1]->InitCamera();
}


void NissanApplication::WindowPostRender(video::RenderWindow* wnd)
{
	video::IRenderTarget* rt;
	if(m_switcher)
		rt=m_cameras[0]->Render();
	else
		rt=m_cameras[1]->Render();
	m_switcher=!m_switcher;
	video::TextureUnit tex;
	tex.SetTexture(rt->getColorTexture());
	getDevice()->set2DMode();
	getDevice()->useTexture(0,&tex);
	getDevice()->draw2DImage(m_viewPort->getAbsViewPort(),1);
}

void NissanApplication::update(float dt)
{
}

void NissanApplication::onDone()
{
}


}