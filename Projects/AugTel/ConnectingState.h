
#ifndef ConnectingState_h__
#define ConnectingState_h__

#include "IRenderingState.h"

#include "GUIManager.h"

#include "SceneManager.h"
#include "ViewPort.h"
#include "GameEntityManager.h"
#include "ISound.h"
#include "ISoundManager.h"
#include "ISoundListener.h"

namespace mray
{
	namespace game
	{
		class VisItemDescComponent;
	}
	namespace GUI
	{
		class GUIConnectingScreenImpl;
	}
namespace AugTel
{

	class ObjectController;
class ConnectingState :public TBee::IRenderingState, public scene::IViewportListener,public game::IGameEntityManagerListener
{
	typedef TBee::IRenderingState Parent;
protected:
	enum EState
	{
		EConnecting,
		EConnected,
		EDone
	}m_state;
	GCPtr<scene::SceneManager> m_sceneManager;
	GCPtr<game::GameEntityManager> m_gameManager;
	GCPtr<sound::ISoundManager> m_soundManager;
	GCPtr<sound::ISound> m_bgmSnd;
	GCPtr<scene::ViewPort> m_viewport[2];
	GCPtr<physics::IPhysicManager> m_phManager;

	GCPtr<GUI::IGUIManager> m_guiManager;
	GCPtr<GUI::IGUIPanelElement> m_guiroot;
	scene::CameraNode* m_camera[2];
	core::string m_model;

	scene::ISceneNode* m_modelParent;

	ObjectController* m_cameraController;

	GUI::GUIConnectingScreenImpl* m_connectScreen;

	std::vector<game::VisItemDescComponent*> m_descItems;
	int m_currItem;
	float m_timeOut;

	void _CreatePhysicsSystem();
public:
	ConnectingState(const core::string& name);
	virtual ~ConnectingState();

	virtual void InitState();
	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual video::IRenderTarget* Render(const math::rectf& rc, TBee::ETargetEye eye);
	virtual void Update(float dt);

	virtual void LoadFromXML(xml::XMLElement* e);

	virtual void OnAddGameEntity(game::GameEntityManager*m, game::GameEntity*e);

	
};

}
}

#endif // ConnectingState_h__
