

#include "stdafx.h"
#include "CameraRenderingState.h"


#include "DirectShowVideoGrabber.h"
#include "FlyCameraVideoGrabber.h"
#include <IThreadManager.h>


namespace mray
{

	typedef video::DirectShowVideoGrabber VCameraType ;

CameraRenderingState::CameraRenderingState()
{
	m_cameraSource.id=0;

	m_texCoordRectSize=1;
	m_VerticalShift=0;

	m_video=new video::VideoGrabberTexture();
}


CameraRenderingState::~CameraRenderingState()
{
	delete m_video;
}



void CameraRenderingState::InitCamera()
{
	m_rtTexture=Engine::getInstance().getDevice()->createEmptyTexture2D(true);
	m_rtTexture->setMipmapsFilter(false);

	m_renderTarget=Engine::getInstance().getDevice()->createRenderTarget("",m_rtTexture,0,0,0);

	m_texCoordRectSize=1;
	video::ICameraVideoGrabber* vdo=0;
	if(m_cameraSource.usb)
	{
		vdo=new video::DirectShowVideoGrabber();
	}
	else vdo=new video::FlyCameraVideoGrabber();

	video::ITexturePtr tex=Engine::getInstance().getDevice()->createEmptyTexture2D(true);

	m_video->Set(vdo,tex);

	vdo->InitDevice(m_cameraSource.id,m_cameraSource.width,m_cameraSource.height,m_cameraSource.fps);
	vdo->Start();
	printf("Cam%d resolution=%dx%d@%d\n",m_cameraSource.id,vdo->GetFrameSize().x,vdo->GetFrameSize().y,vdo->GetFrameRate());


	m_rtTexture->createTexture(math::vector3d(m_cameraSource.width,m_cameraSource.height,1),video::EPixel_R8G8B8);

}

void CameraRenderingState::SetCameraInfo(int id,int w,int h,int fps,bool usb)
{
	m_cameraSource.id=id;
	m_cameraSource.width=w;
	m_cameraSource.height=h;
	m_cameraSource.fps=fps;
	m_cameraSource.usb=usb;
}





void CameraRenderingState::Close()
{
	VCameraType* cam=(VCameraType*)m_video->GetGrabber().pointer();
	cam->Stop();
}
video::IRenderTarget* CameraRenderingState::Render()
{

	float shift=(float)m_VerticalShift*0.5f;

	math::rectf texCoord(1-m_texCoordRectSize,1-m_texCoordRectSize,m_texCoordRectSize,m_texCoordRectSize);

	video::IVideoDevice* dev=Engine::getInstance().getDevice();
	dev->setRenderTarget(m_renderTarget);

	m_video->Blit();

	video::TextureUnit tex;
	tex.SetEdgeColor(video::SColor(0,0,0,1));
	tex.setTextureClamp(video::ETW_WrapR,video::ETC_CLAMP_TO_BORDER);
	tex.setTextureClamp(video::ETW_WrapS,video::ETC_CLAMP_TO_BORDER);
	tex.setTextureClamp(video::ETW_WrapT,video::ETC_CLAMP_TO_BORDER);
	tex.SetTexture(m_video->GetTexture());
	dev->useTexture(0,&tex);
	dev->draw2DImage(math::rectf(math::vector2d(0,shift),math::vector2d(m_cameraSource.width,m_cameraSource.height)+math::vector2d(0,shift)),1,0,&texCoord);

	dev->setRenderTarget(0);
	dev->useTexture(0,0);


	return m_renderTarget;

}


void CameraRenderingState::LoadFromXML(xml::XMLElement* e)
{
	xml::XMLAttribute* attr;


	attr=e->getAttribute("ID");
	if(attr)
		m_cameraSource.id=core::StringConverter::toInt(attr->value);

	attr=e->getAttribute("Width");
	if(attr)
	{
		m_cameraSource.width=core::StringConverter::toInt(attr->value);
	}
	attr=e->getAttribute("Height");
	if(attr)
	{
		m_cameraSource.height=core::StringConverter::toInt(attr->value);
	}
	attr=e->getAttribute("FPS");
	if(attr)
	{
		m_cameraSource.fps=core::StringConverter::toInt(attr->value);
	}
}




}


