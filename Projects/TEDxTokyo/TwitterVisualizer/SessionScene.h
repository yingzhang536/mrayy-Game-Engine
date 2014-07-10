
#ifndef SessionScene_h__
#define SessionScene_h__

#include "IRenderingScene.h"
#include "LeapDevice.h"
#include "ISound.h"

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
		class PointerNode;
	}
namespace ted
{

class TwitterProviderListener;
class SessionScene :public IRenderingScene, public nui::ILeapListener
{
protected:
	GUI::IGUIPanelElement* m_guiroot;
	GUI::GUIManager* m_guiMngr;
	GUI::GUIScreenLayoutImplV2* m_screenLayout;

	scene::ISceneManager* m_sceneManager;

	scene::ViewPort* m_vp;

	scene::SessionRenderer* m_sessionRenderer;
	TwitterProviderListener* m_providerListener;
	scene::PointerNode* m_pointer;

	sound::ISoundPtr m_bgm;

	math::vector3d m_leapVec;
	math::vector3d m_lastSpeedVec;
	math::vector3d m_leapAcc;
	float m_lastHandAppearTime;
	bool m_tapped;
	bool m_grasping;
public:
	SessionScene();
	virtual~SessionScene();


	virtual void Init();
	virtual void OnEnter() ;
	virtual void OnExit() ;

	virtual bool OnEvent(Event* e, const math::rectf& rc) ;
	virtual void Update(float dt) ;
	virtual video::IRenderTarget* Draw(const math::rectf& rc);

	virtual void OnConnected(nui::LeapDevice* d){}
	virtual void OnDisconnected(nui::LeapDevice* d){}
	virtual void OnGesture(nui::LeapDevice* d, nui::ELeapGestureType type, Leap::Gesture g);
	virtual void OnHands(nui::LeapDevice* d, Leap::HandList hands);
};

}
}

#endif // SessionScene_h__
