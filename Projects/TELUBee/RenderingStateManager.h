

/********************************************************************
	created:	2012/10/10
	created:	10:10:2012   10:58
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\RenderingStateManager.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	RenderingStateManager
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___RenderingStateManager___
#define ___RenderingStateManager___

#include <StateMachine.h>
#include "ITransitionBlender.h"
#include "IApplicationState.h"
namespace mray
{

namespace TBee
{

	class Application;
	class IRenderingState;

class RenderingStateManager:public IApplicationState,public IStateMachineListener
{
protected:
	StateMachine* m_stateMachine;
	IRenderingState* m_oldState;
	GCPtr<ITransitionBlender> m_blender;

	Application* m_app;

	IRenderingState* m_introState;
	IRenderingState* m_loadingState;
	IRenderingState* m_loginState;
	IRenderingState* m_cameraState;

	void OnStateChanged(StateMachine*,IState* oldS,IState* newS);

public:
	RenderingStateManager(Application*app);
	virtual~RenderingStateManager();

	void AddState(IRenderingState* st,const core::string&name);

	IRenderingState* GetActiveState();

	virtual void OnEvent(Event* e);
	virtual void OnEnter(IApplicationState* prev);
	virtual void OnExit();

	virtual void OnUpdate(float dt);
	virtual void OnDraw(const math::rectf& rc,video::IRenderTarget* rt,ETargetEye eye);

	void SetInitialState(const core::string&name);
	void AddTransition(const core::string&a,const core::string&b,int code);

	ITransitionBlender* GetBlender(){return m_blender;}

	void LoadSettingsXML(xml::XMLElement* e);


};

}
}

#endif