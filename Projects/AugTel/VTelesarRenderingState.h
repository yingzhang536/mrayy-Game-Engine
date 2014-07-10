

#ifndef VTelesarRenderingState_h__
#define VTelesarRenderingState_h__


#include "IRenderingState.h"
#include "SceneManager.h"
#include "ViewPort.h"
#include "GameEntityManager.h"
#include "ISound.h"
#include "ISoundListener.h"
#include "GUIManager.h"
#include "GUIAugTelScreen.h"


namespace mray
{

class Application;
namespace TBee
{
	class TelubeeCameraConfiguration;
}
namespace VT
{
	class RobotCommunicatorComponent;
}
namespace AugTel
{

	class VTBaseState;
	class VTelesarStateImpl;


class VTelesarRenderingState :public TBee::IRenderingState,public scene::IViewportListener
{
protected:

	enum ECameraMode
	{
		SwitchHead,
		FreeMode,
		HeadFront,
		Side,
		Front,
		Back,
		Object
	};
	GCPtr<GUI::IGUIRenderer> m_guiRenderer;

	GCPtr<scene::SceneManager> m_sceneManager;
	GCPtr<game::GameEntityManager> m_gameManager;
	GCPtr<scene::ViewPort> m_viewport[2];
	GCPtr<physics::IPhysicManager> m_phManager;
	GCPtr<scene::IDebugDrawManager> m_debugRenderer;

	GCPtr<GUI::IGUIManager> m_guiManager;
	GCPtr<GUI::IGUIPanelElement> m_guiroot;
	scene::CameraNode* m_camera[2];

	VTBaseState* m_vtState;

	VTelesarStateImpl* m_data;

	core::string m_model;
	core::string m_environment;
	TBee::TelubeeCameraConfiguration *m_cameraConfiguration;
	VT::RobotCommunicatorComponent* m_robot;
	bool m_attachToHead;

	bool m_loaded;

	bool m_stateActive;

	void _LoadEnvironment(const core::string& path);
	void _Clear();
	void _LoadModel(const core::string& scene);
	void _SwitchCamera(scene::IMovable* m, ECameraMode mode);

	void _CreatePhysicsSystem();
	virtual void _RenderUI(const math::rectf& rc, math::vector2d& pos);

public:
	VTelesarRenderingState(const core::string& name);
	virtual~VTelesarRenderingState();
	virtual void InitState();
	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual video::IRenderTarget* Render(const math::rectf& rc, TBee::ETargetEye eye);
	virtual void Update(float dt);

	virtual void onRenderBegin(scene::ViewPort*vp);
	virtual void onRenderDone(scene::ViewPort*vp);

	virtual void LoadFromXML(xml::XMLElement* e);
};

}
}

#endif // VTelesarRenderingState_h__
