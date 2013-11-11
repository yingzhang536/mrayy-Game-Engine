

/********************************************************************
	created:	2013/02/03
	created:	3:2:2013   13:10
	filename: 	C:\Development\mrayEngine\Projects\TELUBee\NullRenderState.h
	file path:	C:\Development\mrayEngine\Projects\TELUBee
	file base:	NullRenderState
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __NullRenderState__
#define __NullRenderState__


#include "IRenderingState.h"

namespace mray
{
namespace VT
{

class NullRenderState:public IRenderingState
{
protected:
public:
	NullRenderState(){m_exitCode=STATE_EXIT_CODE;}
	virtual~NullRenderState(){}
	virtual video::IRenderTarget* Render(bool left,const math::rectf& rc)
	{
		return m_renderTarget[0];
	}
};

}
}


#endif
