
#ifndef SessionScene_h__
#define SessionScene_h__

#include "IRenderingScene.h"
#include "LeapDevice.h"

namespace mray
{
	namespace GUI
	{
		class IGUIPanelElement;
		class GUIManager;
		class GUIScreenLayoutImplV2;
	}
	namespace scene
	{
		class SessionRenderer;
	}
namespace ted
{


	class SessionScene :public IRenderingScene, public nui::ILeapListener
{
protected:
	GUI::IGUIPanelElement* m_guiroot;
	GUI::GUIManager* m_guiMngr;
	GUI::GUIScreenLayoutImplV2* m_screenLayout;

	scene::ISceneManager* m_sceneManager;

	scene::CameraNode* m_camera;
	scene::ViewPort* m_vp;

	scene::SessionRenderer* m_sessionRenderer;

public:
	SessionScene();
	virtual~SessionScene();


	virtual void Init();
	virtual void OnEnter() ;
	virtual void OnExit() ;

	virtual bool OnEvent(Event* e, const math::rectf& rc) ;
	virtual void Update(float dt) ;
	virtual video::IRenderTarget* Draw(const math::rectf& rc);
};

}
}

#endif // SessionScene_h__
