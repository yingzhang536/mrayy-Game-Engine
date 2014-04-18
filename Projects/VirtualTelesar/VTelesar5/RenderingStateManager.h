

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
#include "RenderBlender.h"
#include "IApplicationState.h"

namespace mray
{

namespace VT
{
	class IRenderingState;
	class Application;

	class IRenderStateManagerListener
	{
	public:
		virtual void OnStateChanged(IRenderingState* old,IRenderingState* state){}
	};

class RenderingStateManager:public IApplicationState,public IStateMachineListener,public ListenerContainer<IRenderStateManagerListener*>
{
protected:
	StateMachine* m_stateMachine;
	IRenderingState* m_oldState;
	RenderBlender m_blender;

	Application* m_app;


	void OnStateChanged(StateMachine*,IState* oldS,IState* newS);

	DECLARE_FIRE_METHOD(OnStateChanged,(IRenderingState* old,IRenderingState* state),(old,state));

public:
	RenderingStateManager(Application*app);
	virtual~RenderingStateManager();

	void AddState(IRenderingState* st,const core::string&name);

	IRenderingState* GetActiveState();

	virtual void OnEvent(Event* e);
	virtual void OnEnter(IApplicationState* prev);
	virtual void OnExit();

	virtual void OnUpdate(float dt);
	virtual void OnDraw(bool left,const math::rectf& rc,video::IRenderTarget* rt);

	void SetInitialState(const core::string&name);
	void AddTransition(const core::string&a,const core::string&b,int code);

	RenderBlender& GetBlender(){return m_blender;}

	void LoadScenario(const core::string& fname);

	void SetStaticBlending(bool e,float a,float b);
	void SetBlendingSpeed(float s);


};

}
}

#endif
