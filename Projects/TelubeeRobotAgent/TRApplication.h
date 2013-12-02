

/********************************************************************
	created:	2013/12/01
	created:	1:12:2013   23:23
	filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\TRApplication.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
	file base:	TRApplication
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __TRApplication__
#define __TRApplication__

#include "CMRayApplication.h"
#include "GUIBatchRenderer.h"
#include "ICameraVideoGrabber.h"
#include "VideoGrabberTexture.h"
#include "GstVideoProvider.h"

namespace mray
{

class TRApplication :public CMRayApplication
{
protected:
	scene::ViewPort* m_viewPort;

	GCPtr<GUI::GUIBatchRenderer> m_guiRender;

	GCPtr<video::ICameraVideoGrabber> m_cameras[2];
	GCPtr<video::IVideoGrabber> m_combinedCameras;

	video::VideoGrabberTexture m_cameraTextures[3];

	GstVideoProvider* m_videoProvider;

	void _InitResources();
public:
	TRApplication();
	virtual~TRApplication();


	virtual void onEvent(Event* event);

	virtual void init(const OptionContainer &extraOptions);

	virtual void draw(scene::ViewPort* vp);
	virtual void WindowPostRender(video::RenderWindow* wnd);
	virtual void update(float dt);
	virtual void onDone();

	virtual void onRenderDone(scene::ViewPort*vp);

};

}


#endif
