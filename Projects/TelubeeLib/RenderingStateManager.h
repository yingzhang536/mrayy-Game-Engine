

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

	class IRenderStateManagerListener
	{
	public:
		virtual void OnStateChanged(TBee::IRenderingState* old, TBee::IRenderingState* state){};

	};

	class RenderingStateManager :public IApplicationState, public IStateMachineListener, public ListenerContainer<IRenderStateManagerListener*>
{
protected:
	StateMachine* m_stateMachine;
	IRenderingState* m_oldState;
	GCPtr<ITransitionBlender> m_blender;

	float m_sleepTime;

	void OnStateChanged(StateMachine*,IState* oldS,IState* newS);
	DECLARE_FIRE_METHOD(OnStateChanged, (TBee::IRenderingState* old, TBee::IRenderingState* state), (old, state))
public:
	RenderingStateManager();
	virtual~RenderingStateManager();

	void AddState(IRenderingState* st);
	IRenderingState* GetActiveState();

	void SetSleepTime(float s){ m_sleepTime = s; }

	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void OnEnter(IApplicationState* prev);
	virtual void OnExit();

	virtual void OnUpdate(float dt);
	virtual void OnDraw(const math::rectf& rc,video::IRenderTarget* rt,ETargetEye eye);

	void SetInitialState(IRenderingState* st);
	void AddTransition(IRenderingState* a, IRenderingState* b, int code);
	void InitStates();
	ITransitionBlender* GetBlender(){return m_blender;}



	void LoadSettingsXML(xml::XMLElement* e);
	void WriteSettingsXML(xml::XMLElement* e);


};

}
}

#endif
