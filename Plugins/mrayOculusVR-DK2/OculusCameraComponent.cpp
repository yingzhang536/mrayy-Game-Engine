

#include "stdafx.h"
#include "OculusCameraComponent.h"
#include "OculusDevice.h"
#include "CameraNode.h"
#include "ViewPort.h"
#include "ParsedShaderPP.h"


namespace mray
{
namespace game
{


IMPLEMENT_RTTI(OculusCameraComponent,IGameComponent);

OculusCameraComponent::OculusCameraComponent(game::GameEntityManager*mngr)
{
	m_shader=0;
	m_device=0;
	m_vp=0;
	m_isDirty=false;
	m_perspOffset=0;
}
OculusCameraComponent::~OculusCameraComponent()
{
}

bool OculusCameraComponent::InitComponent()
{
	return game::IGameComponent::InitComponent();

}
void OculusCameraComponent::Update(float dt)
{
	IGameComponent::Update(dt);
	//if(!m_isDirty || !m_device || !m_vp || !m_shader)
	//	return;
	if (!m_device || !m_device->IsConnected() || !m_vp)
		return;
	
	m_isDirty=false;
	

}
void OculusCameraComponent::InitCamera(video::OculusDevice* device,scene::ViewPort* vp,ETargetCamera target,video::ParsedShaderPP* shader)
{
	m_device=device;
	m_vp=vp;
	m_target=target;
	m_shader=shader;
	m_isDirty=true;
}


IMPLEMENT_GAMECOMPONENT_FACTORY(OculusCameraComponent);


}
}
