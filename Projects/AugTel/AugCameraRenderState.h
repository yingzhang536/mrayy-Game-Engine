

/********************************************************************
	created:	2014/01/09
	created:	9:1:2014   2:30
	filename: 	C:\Development\mrayEngine\Projects\AugTel\AugCameraRenderState.h
	file path:	C:\Development\mrayEngine\Projects\AugTel
	file base:	AugCameraRenderState
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __AugCameraRenderState__
#define __AugCameraRenderState__

#include "IEyesRenderingBaseState.h"
#include "SceneManager.h"
#include "ViewPort.h"
#include "GameEntityManager.h"
#include "ISound.h"
#include "ISoundListener.h"
#include "GUIManager.h"

#include "IGUISliderBar.h"


namespace mray
{
	namespace TBee
	{
		class OpenNIHandler;
		class DepthVisualizer;
	}
	using namespace TBee;
namespace AugTel
{

	class AugCameraStateImpl;
class AugCameraRenderState :public TBee::IEyesRenderingBaseState,public scene::IViewportListener
{
	typedef TBee::IEyesRenderingBaseState Parent;
protected:
	GCPtr<scene::SceneManager> m_sceneManager;
	GCPtr<game::GameEntityManager> m_gameManager;
	GCPtr<scene::ViewPort> m_viewport[2];
	GCPtr<physics::IPhysicManager> m_phManager;
	GCPtr<scene::IDebugDrawManager> m_debugRenderer;

	GCPtr<GUI::IGUIManager> m_guiManager;
	GCPtr<GUI::IGUIPanelElement> m_guiroot;
	scene::CameraNode* m_camera[2];
	
	AugCameraStateImpl* m_data;

	TBee::ICameraVideoSource* m_camVideoSrc;

	core::string m_optiProvider;
	core::string m_model;

	TBee::OpenNIHandler* m_openNiHandler;

	scene::SMeshPtr m_depthMesh;
	scene::ISceneNode* m_depthNode;

	TBee::DepthVisualizer* m_depthVisualizer;
	bool m_viewDepth;

	float m_depthTime;
	bool m_showScene;

	void _CalculateDepthGeom();
	void _CreatePhysicsSystem();
public:
	AugCameraRenderState(TBee::ICameraVideoSource* src);
	virtual~AugCameraRenderState();

	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void InitState();
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual video::IRenderTarget* Render(const math::rectf& rc, ETargetEye eye);
	virtual void Update(float dt);
	//virtual video::IRenderTarget* GetLastFrame(ETargetEye eye){ return m_viewport->getRenderTarget(); }
	virtual bool CanSleep(){ return false; }

	virtual void onRenderBegin(scene::ViewPort*vp);
	virtual void onRenderDone(scene::ViewPort*vp);

	virtual void LoadFromXML(xml::XMLElement* e);


};

}
}


#endif
