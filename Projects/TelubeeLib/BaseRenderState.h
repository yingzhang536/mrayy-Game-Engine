


/********************************************************************
	created:	2012/10/10
	created:	10:10:2012   17:24
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\BaseRenderState.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	BaseRenderState
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___BaseRenderState___
#define ___BaseRenderState___

#include "IState.h"

namespace mray
{
namespace TBee
{

	class IRenderingState;

class BaseRenderState:public IState
{
protected:
	IRenderingState* m_state;
public:
	BaseRenderState(const core::string&name,IRenderingState*s):IState(name),m_state(s){}
	virtual~BaseRenderState();

	IRenderingState* GetState(){return m_state;}

	virtual void onEnter(IState*prev);
	virtual void onUpdate();
	virtual void onExit();
	
};

}
}

#endif
