

/********************************************************************
	created:	2012/10/10
	created:	10:10:2012   18:39
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\CameraRenderingState.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	CameraRenderingState
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___CameraRenderingState___
#define ___CameraRenderingState___


#include "VideoGrabberTexture.h"
#include "IRenderTarget.h"



namespace mray
{

class CameraRenderingState
{
protected:

	video::ITexturePtr m_rtTexture;
	video::IRenderTargetPtr m_renderTarget;

	struct CameraInfo
	{
		CameraInfo():id(0),width(320),height(240),fps(25),usb(true)
		{
		}
		int id;
		int width;
		int height;
		int fps;
		bool usb;
	};

	CameraInfo m_cameraSource;
	video::VideoGrabberTexture* m_video;

	int m_VerticalShift;

	float m_texCoordRectSize;

public:
	CameraRenderingState();
	virtual~CameraRenderingState();

	virtual void InitCamera();
	void Close();
	void SetCameraInfo(int id,int w,int h,int fps,bool usb=true);

	virtual video::IRenderTarget* Render();

	virtual void LoadFromXML(xml::XMLElement* e);
	virtual bool CanSleep(){return false;}

};
}

#endif
