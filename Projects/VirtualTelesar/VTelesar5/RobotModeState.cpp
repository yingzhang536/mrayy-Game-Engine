

#include "stdafx.h"
#include "RobotModeState.h"




namespace mray
{
namespace VT
{


RobotModeState::RobotModeState()
{
}

RobotModeState::~RobotModeState()
{
}


void RobotModeState::InitState(Application* app)
{
	IRenderingState::InitState(app);
}

void RobotModeState::OnEvent(Event* e)
{
}

void RobotModeState::OnEnter(IRenderingState*prev)
{
}
void RobotModeState::OnExit()
{
	IRenderingState::OnExit();
}

void RobotModeState::Update(float dt)
{
}

video::IRenderTarget* RobotModeState::Render(bool left,const math::rectf& rc)
{
	return IRenderingState::Render(left,rc);
}


void RobotModeState::LoadFromXML(xml::XMLElement* e)
{
}


}
}
