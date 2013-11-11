

#include "stdafx.h"
#include "BaseRenderState.h"
#include "IRenderingState.h"


namespace mray
{
namespace VT
{

BaseRenderState::~BaseRenderState()
{
	delete m_state;
}


void BaseRenderState::onEnter(IState*prev)
{
	IRenderingState*p=((BaseRenderState*)prev)->GetState();
	m_state->OnEnter(p);
}
void BaseRenderState::onUpdate()
{
}
void BaseRenderState::onExit()
{
}

}
}
