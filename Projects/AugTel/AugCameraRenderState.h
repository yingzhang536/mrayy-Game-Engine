

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
#include "GUIAugTelScreen.h"
#include "IGUISliderBar.h"
#include "DataCommunicator.h"


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
	class VTBaseState;
class AugCameraRenderState :public TBee::IEyesRenderingBaseState,public scene::IViewportListener,public IDataCommunicatorListener
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

	VTBaseState* m_vtState;
	
	AugCameraStateImpl* m_data;

	TBee::ICameraVideoSource* m_camVideoSrc;

	core::string m_optiProvider;
	core::string m_model;

	TBee::OpenNIHandler* m_openNiHandler;

	math::vector4d m_depthParams;

	scene::SMeshPtr m_depthMesh;
	scene::ISceneNode* m_depthNode;
	GCPtr<GUI::GUIAugTelScreen> m_screenLayout;

	TBee::DepthVisualizer* m_depthVisualizer;

	video::ParsedShaderPP* m_blurShader;

	bool m_viewDepth;

	bool m_takeScreenShot;

	float m_depthTime;
	bool m_showScene;

	float m_focus;

	bool m_showDebug;

	float m_lightMapTimer;
	scene::LightNode* m_lightSrc;

	void _CalculateDepthGeom();
	void _CreatePhysicsSystem();

	void _GenerateLightMap();
	virtual void _RenderUI(const math::rectf& rc, math::vector2d& pos);
public:
	AugCameraRenderState(TBee::ICameraVideoSource* src, TBee::IRobotCommunicator* comm, const core::string& name);
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

	virtual void OnDepthData(const TBee::GeomDepthRect& dpRect);
	virtual void OnDepthSize(const math::vector2di &sz);
	virtual void OnIsStereoImages(bool isStereo);
	virtual void OnCameraConfig(const core::string& cameraProfile);

};

}
}


#endif
