

#include "stdafx.h"
#include "BaseRenderState.h"
#include "IRenderingState.h"


namespace mray
{
namespace TBee
{
	BaseRenderState::BaseRenderState(const core::string&name, IRenderingState*s) :IState(name), m_state(s)
	{
		m_state->SetName(name);
	}
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
