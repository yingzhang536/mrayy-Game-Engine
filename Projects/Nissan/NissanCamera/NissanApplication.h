

/********************************************************************
	created:	2013/05/28
	created:	28:5:2013   21:47
	filename: 	C:\Development\mrayEngine\Projects\Nissan\NissanCamera\NissanApplication.h
	file path:	C:\Development\mrayEngine\Projects\Nissan\NissanCamera
	file base:	NissanApplication
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __NissanApplication__
#define __NissanApplication__


#include "CMRayApplication.h"

#include "ViewPort.h"
#include "CameraRenderingState.h"



namespace mray
{

class NissanApplication:public CMRayApplication
{
protected:
public:

	scene::ViewPort* m_viewPort;
	GCPtr<CameraRenderingState> m_cameras[2];

	bool m_switcher;

public:
	NissanApplication();
	virtual~NissanApplication();

	virtual void onEvent(Event* event);

	virtual void init(const OptionContainer &extraOptions);

	//virtual void draw(scene::ViewPort* vp);
	virtual void WindowPostRender(video::RenderWindow* wnd);
	virtual void update(float dt);
	virtual void onDone();

};


}


#endif
