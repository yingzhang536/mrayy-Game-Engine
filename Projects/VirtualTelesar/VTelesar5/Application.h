

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
#include "StereoRenderer.h"

#include <ISoundManager.h>
#include <IVideoClipManager.h>

#include "IRenderingState.h"
#include "RenderBlender.h"
#include "RenderingStateManager.h"

#include "BenchmarkItem.h"

namespace mray
{
namespace VT
{

class Application:public CMRayApplication,public BenchmarkItem,public VT::IRenderStateManagerListener
{
protected:
	scene::ViewPort* m_viewPort;

	GCPtr<GUI::GUIBatchRenderer> m_guiRender;

	GCPtr<scene::StereoRenderer> m_stereoRenderer;

	GCPtr<sound::ISoundManager> m_soundManager;
	GCPtr<video::IVideoClipManager> m_videoManager;

	video::ITexturePtr m_screenShot;


	scene::ViewPort* m_mainVP[2];
	scene::ViewPort* m_stereoVP[2];
	video::RenderWindow* m_2ndWnd;

	video::RenderWindow* m_previewWnd;
	bool m_drawUI;
	bool m_flipEyes;
	GCPtr<VT::RenderingStateManager> m_renderingStateManager;

	video::ITexturePtr m_rtTexture[2];
	video::IRenderTargetPtr m_renderTarget[2];;

	bool m_horizontalFlip;

	void _InitResources();
	void RenderUI(const math::rectf& rc);
	

	BenchmarkItem *m_renderBenchMark;
	BenchmarkItem *m_updateBenchMark;

public:
	Application();
	virtual~Application();

	virtual void onEvent(Event* event);

	virtual void init(const OptionContainer &extraOptions);

	int getEyeIndex(bool left);

	scene::StereoRenderer* GetStereoRenderer(){return m_stereoRenderer;}


	//virtual void draw(scene::ViewPort* vp);
	virtual void WindowPostRender(video::RenderWindow* wnd);
	virtual void update(float dt);
	virtual void onDone();

	scene::ViewPort* GetViewport(bool left);


	bool IsStereo();

	GCPtr<video::IVideoClipManager> GetVideoManager(){return m_videoManager;}
	video::RenderWindow* Get2ndRenderWindow(){return m_2ndWnd;}


	void OnStateChanged(IRenderingState* old,IRenderingState* state);

};

}
}


#endif
