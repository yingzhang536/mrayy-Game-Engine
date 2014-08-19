

/********************************************************************
created:	2012/10/09
created:	9:10:2012   22:07
filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\Application.h
file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
file base:	Application
file ext:	h
author:		MHD YAMEN SARAIJI

purpose:
*********************************************************************/
#ifndef ___Application___
#define ___Application___



#include "CMRayApplication.h"

#include "GUIBatchRenderer.h"
#include "GUIManager.h"
#include "ViewPort.h"

#include <ISoundManager.h>
#include "OculusManager.h"
#include "OculusDevice.h"
#include "ParsedShaderPP.h"

namespace mray
{

class Application :public CMRayApplication
{
protected:
	scene::ViewPort* m_viewPort[2];
	GCPtr<video::ParsedShaderPP> oculusRenderer[2];

	GCPtr<GUI::GUIBatchRenderer> m_guiRender;
	GCPtr<sound::ISoundManager> m_soundManager;

	GCPtr<video::OculusManager> m_oculusMngr;
	video::OculusDevice* m_oculusDevice;

	GCPtr<scene::SceneManager> m_sceneManager;
	scene::ISceneNode* m_headNode;

public:
	Application();
	virtual~Application();

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
