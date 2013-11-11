
#include "stdafx.h"
#include "HMDCameraRenderer.h"
#include "CameraRenderingState.h"
#include "IRenderArea.h"


namespace mray
{
namespace TBee
{

HMDCameraRenderer::HMDCameraRenderer()
{
	m_camera=new CameraRenderingState();
}

HMDCameraRenderer::~HMDCameraRenderer()
{
	delete m_camera;
}

void HMDCameraRenderer::SetCameraInfo(ETargetEye eye,const HMDCameraInfo& ifo)
{
	m_info[eye==Eye_Left?0:1]=ifo;
	m_camera->SetCameraInfo(eye,ifo.id);
}

void HMDCameraRenderer::Init(Application*app)
{
	m_camera->InitState(app);
}
void HMDCameraRenderer::Grab(const math::rectf &rc,ETargetEye eye)
{

	m_camera->Render(math::rectf(0,rc.getSize()),eye);
}
void HMDCameraRenderer::Render(const math::rectf &rc,ETargetEye eye)
{

	m_tUnit.SetTexture(m_camera->GetLastFrame(eye)->getColorTexture());
	Engine::getInstance().getDevice()->useTexture(0,&m_tUnit);;
	Engine::getInstance().getDevice()->draw2DImage(rc,1);
}

void HMDCameraRenderer::Update(float dt)
{
	m_camera->Update(dt);
}

void HMDCameraRenderer::Enter()
{
	m_camera->OnEnter(0);
}

void HMDCameraRenderer::Exit()
{
	m_camera->OnExit();
}

void HMDCameraRenderer::LoadFromXML(xml::XMLElement* e)
{
	m_camera->LoadFromXML(e);
}


}
}
