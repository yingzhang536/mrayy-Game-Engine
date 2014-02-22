

#include "stdafx.h"
#include "GSTCameraRenderingState.h"
#include "RobotInfoManager.h"
#include "FontResourceManager.h"
#include "CRobotConnector.h"
#include "AppData.h"
#include "IGUIRenderer.h"
#include "GStreamVideoProvider.h"

#include "TextureResourceManager.h"
#include "GstNetVideoSource.h"

namespace mray
{
namespace TBee
{


GSTCameraRenderingState::GSTCameraRenderingState()
{

	m_fps = 0;
	m_timeAcc = 0;
	IEyesRenderingBaseState::InitState();
	

	m_cameraSource = new GstNetVideoSource();
// 	m_eyes[0].flip90 = true;
// 	m_eyes[1].flip90 = true;
 	m_eyes[0].cw = true;
 	m_eyes[1].cw = false;

	SetVideoSource(m_cameraSource);

}


GSTCameraRenderingState::~GSTCameraRenderingState()
{
	delete m_cameraSource;
}


void GSTCameraRenderingState::InitState()
{
	m_cameraSource->Init();

}



bool GSTCameraRenderingState::OnEvent(Event* e, const math::rectf &rc)
{
	if (IEyesRenderingBaseState::OnEvent(e, rc))
		return true;
	bool ok = false;

	if (e->getType() == ET_Keyboard)
	{
		KeyboardEvent* evt = (KeyboardEvent*)e;
		if (evt->press)
		{
			if (evt->key == KEY_ESCAPE)
			{
				m_exitCode = STATE_EXIT_CODE;
				m_robotConnector->EndUpdate();
				ok = true;
			}
			else if (evt->key == KEY_SPACE)
			{
				if (m_robotConnector->IsRobotConnected())
					m_robotConnector->EndUpdate();
				else
					m_robotConnector->StartUpdate();
				ok = true;
			}
		}
	}
	return ok;
}


void GSTCameraRenderingState::OnEnter(IRenderingState*prev)
{

	IEyesRenderingBaseState::OnEnter(prev);
	TBRobotInfo* ifo = AppData::Instance()->robotInfoManager->GetRobotInfo(0);

	if (ifo)
	{
		m_cameraSource->SetIP(ifo->IP);
		m_cameraSource->Open();
		m_robotConnector->ConnectRobotIP(ifo->IP, 5000, 5002, 5003);
	}
	m_frameCounter = 0;
}


void GSTCameraRenderingState::OnExit()
{
	IEyesRenderingBaseState::OnExit();
	m_cameraSource->Close();
}


void GSTCameraRenderingState::Update(float dt)
{
	IEyesRenderingBaseState::Update(dt);
	if (m_cameraSource->Blit())
	{
		m_frameCounter++;
		if (m_timeAcc >= 1)
		{
			m_fps = m_frameCounter;
			m_frameCounter = 0;
			m_timeAcc -= 1;
		}
		_UpdateCameraParams();
	}
	m_timeAcc += dt;
}

void GSTCameraRenderingState::_RenderUI(const math::rectf& rc)
{
	IEyesRenderingBaseState::_RenderUI(rc);

	GUI::IFont* font = gFontResourceManager.getDefaultFont();
	GUI::FontAttributes attr;
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	if (font)
	{
		attr.fontColor.Set(1, 1, 1, 1);
		attr.fontAligment = GUI::EFA_MiddleLeft;
		attr.fontSize = 18;
		attr.hasShadow = true;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;

		math::rectf r = rc;

		core::string msg = mT("FPS:")+core::StringConverter::toString(m_fps);
		font->print(r, &attr, 0, msg, m_guiRenderer);
		r.ULPoint.y += attr.fontSize + 5;
// 		msg=mT("Bytes:") + core::StringConverter::toString(m_providers->GetRecvBytesPerSecond());
// 		font->print(r, &attr, 0, msg, m_guiRenderer);

		m_guiRenderer->Flush();
	}

}

video::IRenderTarget* GSTCameraRenderingState::Render(const math::rectf& rc, ETargetEye eye)
{
	video::IRenderTarget*ret = IEyesRenderingBaseState::Render(rc, eye);
	//draw a grid
	video::IVideoDevice* dev = Engine::getInstance().getDevice();


//	_RenderUI(rc);
 //	dev->setRenderTarget(0);
// 	dev->useTexture(0, 0);


	return 0;
}



void GSTCameraRenderingState::LoadFromXML(xml::XMLElement* e)
{
	IEyesRenderingBaseState::LoadFromXML(e);
	xml::XMLAttribute* attr;


	attr = e->getAttribute("Host");
	if (attr)
		m_hostIp = attr->value;
	attr = e->getAttribute("HostPort");
	if (attr)
		m_hostPort = core::StringConverter::toInt(attr->value);
	attr = e->getAttribute("LocalPort");
	if (attr)
		m_localPort = core::StringConverter::toInt(attr->value);
}



}
}
