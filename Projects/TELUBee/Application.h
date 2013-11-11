

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
#include "GameEntityManager.h"
#include "ViewPort.h"

#include <ISoundManager.h>
#include <IVideoClipManager.h>

#include "IRenderingState.h"
#include "RenderBlender.h"
#include "RenderingStateManager.h"

#include "ApplicationStateManager.h"

namespace mray
{
namespace TBee
{
	class RenderingStateManager;
	class SeeThroughWindow;

	class ApplicationOculusData;

class Application:public CMRayApplication,public scene::IViewportListener
{
protected:
	scene::ViewPort* m_viewPort[2];

	GCPtr<GUI::GUIBatchRenderer> m_guiRender;
	GCPtr<sound::ISoundManager> m_soundManager;
	GCPtr<video::IVideoClipManager> m_videoManager;

	video::ITexturePtr m_screenShot;

	GCPtr<TBee::ApplicationStateManager> m_appStateManager;
	TBee::RenderingStateManager* m_renderingState;

	video::ITexturePtr m_rtTexture;
	video::IRenderTargetPtr m_renderTarget;;

	SeeThroughWindow* m_seeThroughWindow;

	bool m_drawUI;

	ApplicationOculusData* m_oculusData;

	void _InitResources();
	void RenderUI(scene::ViewPort* vp);

	void LoadSettingsXML(const core::string &path);
	void _RenderVP(int i);
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

	GCPtr<video::IVideoClipManager> GetVideoManager(){return m_videoManager;}
};

}
}
#endif
