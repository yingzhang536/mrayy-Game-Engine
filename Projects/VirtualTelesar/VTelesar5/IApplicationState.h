

/********************************************************************
	created:	2012/10/10
	created:	10:10:2012   11:07
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\IApplicationState.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	IApplicationState
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___IApplicationState___
#define ___IApplicationState___


namespace mray
{
namespace VT
{

class IApplicationState
{
protected:

	int m_exitCode;
public:
	IApplicationState():m_exitCode(0){}
	virtual~IApplicationState(){}

	virtual void OnEvent(Event* e)=0;
	virtual void OnEnter(IApplicationState* prev)=0;
	virtual void OnExit(){m_exitCode=0;}

	virtual void OnUpdate(float dt)=0;
	virtual void OnDraw(bool left,const math::rectf& rc,video::IRenderTarget* rt)=0;

	uint GetExitCode(){return m_exitCode;}
};

}
}

#endif
