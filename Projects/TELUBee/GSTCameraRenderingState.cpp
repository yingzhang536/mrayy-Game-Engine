

#include "stdafx.h"
#include "GSTCameraRenderingState.h"
#include "Application.h"
#include "TBAppGlobals.h"
#include "RobotInfoManager.h"
#include "CRobotConnector.h"
#include "FontResourceManager.h"

namespace mray
{
namespace TBee
{


GSTCameraRenderingState::GSTCameraRenderingState()
{
	m_providers = new GStreamVideoProvider();
	m_fps = 0;
	m_timeAcc = 0;

// 	m_eyes[0].flip90 = true;
// 	m_eyes[1].flip90 = true;
// 	m_eyes[0].cw = true;
// 	m_eyes[1].cw = false;

}


GSTCameraRenderingState::~GSTCameraRenderingState()
{
	m_providers->Disconnect();
	delete m_providers;
}

math::vector2d GSTCameraRenderingState::_GetEyeResolution(int i)
{
	math::vector3di sz= m_remoteTex->getSize();
	return math::vector2d(sz.x / 2, sz.y);
}
video::ITexturePtr GSTCameraRenderingState::GetEyeTexture(int i)
{
	return m_remoteTex;
}


void GSTCameraRenderingState::InitState(Application* app)
{
	m_remoteTex = app->getDevice()->createEmptyTexture2D(true);

	IEyesRenderingBaseState::InitState(app);
}



void GSTCameraRenderingState::OnEvent(Event* e)
{
	IEyesRenderingBaseState::OnEvent(e);

	if (e->getType() == ET_Keyboard)
	{
		KeyboardEvent* evt = (KeyboardEvent*)e;
		if (evt->press)
		{
			if (evt->key == KEY_ESCAPE)
			{
				m_exitCode = STATE_EXIT_CODE;
				m_robotConnector->EndUpdate();
			}
			else if (evt->key == KEY_SPACE)
			{
				if (m_robotConnector->IsRobotConnected())
					m_robotConnector->EndUpdate();
				else
					m_robotConnector->StartUpdate();
			}
		}
	}
}


void GSTCameraRenderingState::OnEnter(IRenderingState*prev)
{
	IEyesRenderingBaseState::OnEnter(prev);
	m_providers->ConnectToCameras("192.168.10.46", 5000, 5002, 5001);

	TBRobotInfo* ifo = TBAppGlobals::robotInfoManager->GetRobotInfo(0);
	if (ifo)
	{
		m_robotConnector->ConnectRobotIP(ifo->IP,5000,5002);
	}
	m_frameCounter = 0;
}


void GSTCameraRenderingState::OnExit()
{
	IEyesRenderingBaseState::OnExit();
	m_providers->Disconnect();
}


void GSTCameraRenderingState::Update(float dt)
{
	IEyesRenderingBaseState::Update(dt);
	m_timeAcc+=dt;
	m_providers->Update(dt);
	if (m_providers->HasNewImage(0))
	{
		m_frameCounter++;
		if (m_timeAcc >= 1)
		{
			m_fps = m_frameCounter;
			m_frameCounter = 0;
			m_timeAcc -= 1;
		}
		const video::ImageInfo* image = m_providers->GetImage(0);
		bool dirty = false;
		if (m_remoteTex->getSize() != image->Size)
			dirty = true;
		m_remoteTex->createTexture(math::vector3d(image->Size.x, image->Size.y, 1), image->format);
		if (dirty)
			_UpdateCameraParams();
		video::LockedPixelBox box(math::box3d(0, image->Size), image->format, image->imageData);
		m_remoteTex->getSurface(0)->blitFromMemory(box);
	}
}

void GSTCameraRenderingState::_RenderUI(const math::rectf& rc)
{

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
		m_guiRenderer->Flush();
	}

}

video::IRenderTarget* GSTCameraRenderingState::Render(const math::rectf& rc, ETargetEye eye)
{

	video::IRenderTarget*ret = IEyesRenderingBaseState::Render(rc, eye);
	//draw a grid
	video::IVideoDevice* dev = Engine::getInstance().getDevice();



	//_RenderUI(rc);
	dev->setRenderTarget(0);
	dev->useTexture(0, 0);


	return ret;
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

