
#include "stdafx.h"
#include "IRobotControlState.h"
#include "CRobotConnector.h"


namespace mray
{
namespace TBee
{


IRobotControlState::IRobotControlState(const core::string& name)
	:IEyesRenderingBaseState(name)
{

	m_robotConnector = new CRobotConnector();
	CRobotConnector::InitController(m_robotConnector);
}
IRobotControlState::~IRobotControlState()
{
	delete m_robotConnector;
}

void IRobotControlState::OnEnter(IRenderingState*prev)
{
	IEyesRenderingBaseState::OnEnter(prev);
	m_robotConnector->ConnectRobot();
	m_robotConnector->EndUpdate();
}
void IRobotControlState::OnExit()
{
	IEyesRenderingBaseState::OnExit();
	m_robotConnector->DisconnectRobot();

}
void IRobotControlState::Update(float dt)
{
	IEyesRenderingBaseState::Update(dt);

	m_robotConnector->UpdateStatus();
}

}
}

