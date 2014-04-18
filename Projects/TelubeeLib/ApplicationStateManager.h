

/********************************************************************
	created:	2012/10/10
	created:	10:10:2012   10:55
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\ApplicationStateManager.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	ApplicationStateManager
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___ApplicationStateManager___
#define ___ApplicationStateManager___

#include "IRenderingState.h"


namespace mray
{
	class StateMachine;

namespace TBee
{

	class IApplicationState;

class ApplicationStateManager
{
protected:
	StateMachine* m_stateMachine;
public:
	ApplicationStateManager();
	virtual~ApplicationStateManager();
	
	void AddState(IApplicationState* st,const core::string&name);

	IApplicationState* GetActiveState();

	virtual bool OnEvent(Event* e, const math::rectf& rc);

	void SetInitialState(const core::string&name);
	void AddTransition(const core::string&a,const core::string&b,int code);

	void Update(float dt);
	void Draw(const math::rectf& rc,video::IRenderTarget* rt,ETargetEye eye);

	
};

}
}

#endif
