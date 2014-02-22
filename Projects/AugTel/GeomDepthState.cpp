
#include "stdafx.h"
#include "GeomDepthState.h"
#include "OpenNIHandler.h"
#include "TextureResourceManager.h"

#include "ViewPort.h"
#include "FontResourceManager.h"
#include "GUIBatchRenderer.h"


namespace mray
{
namespace AugTel
{

GeomDepthState::GeomDepthState()
{
	m_openNiHandler = new TBee::OpenNIHandler();
}

GeomDepthState::~GeomDepthState()
{
	delete m_openNiHandler;
}

void GeomDepthState::InitState()
{
	IRenderingState::InitState();
	m_openNiHandler->Init();
	m_openNiHandler->GetNormalCalculator().SetAmplitude(0.5f);
	m_mouseDown = false;
	m_openNiHandler->SetCalculateNormals(false);

	GUI::GUIBatchRenderer*r = new GUI::GUIBatchRenderer();
	r->SetDevice(Engine::getInstance().getDevice());
	m_guiRenderer = r;
}

bool GeomDepthState::OnEvent(Event* e, const math::rectf& rc)
{
	bool ok = false;
	if (e->getType() == ET_Keyboard)
	{
		KeyboardEvent* evt = (KeyboardEvent*)e;
		if (evt->press && evt->key == KEY_D)
		{
			//m_openNiHandler->SetViewDepth(!m_openNiHandler->IsViewDepth());
			ok = true;
		}
		else if (evt->press && evt->key == KEY_F10)
		{
			core::string name = "screenShots/ScreenShot_";
			name += core::StringConverter::toString(gTimer.getActualTime());
			name += ".png";
			gTextureResourceManager.writeResourceToDist(m_rtTexture[0], name);

			ok = true;

		}
		else if (evt->press &&evt->key == KEY_ESCAPE)
		{
			m_exitCode = STATE_EXIT_CODE;
			ok = true;
		}
	}
	else if (e->getType() == ET_Mouse)
	{
		MouseEvent* evt = (MouseEvent*)e;
		if (evt->event == MET_LEFTDOWN)
		{
			m_mouseDown = true;
		}
		else if(evt->event == MET_LEFTUP)
			m_mouseDown = false;
		if (evt->event == MET_MOVED)
		{
			if (m_openNiHandler->IsStarted() && evt->GetViewPort() )
			{
				m_mousePos = evt->pos / evt->GetViewPort()->getSize();
				m_mousePos.x *= m_openNiHandler->GetNormalCalculator().Width();
				m_mousePos.y *= m_openNiHandler->GetNormalCalculator().Height();
			}
		}
	}
	return ok;
}

void GeomDepthState::OnEnter(IRenderingState*prev)
{
	IRenderingState::OnEnter(prev);
	m_openNiHandler->Start(320,240);
}

void GeomDepthState::OnExit()
{
	IRenderingState::OnExit();
	m_openNiHandler->Close();
}

video::IRenderTarget* GeomDepthState::Render(const math::rectf& rc, TBee::ETargetEye eye)
{
	video::IRenderTarget* rt = IRenderingState::Render(rc, eye);
	//m_depthCalculator->SetDepthFrame(m_openNiHandler);
	Engine::getInstance().getDevice()->setRenderTarget(rt);
//	m_openNiHandler->Render(rc);

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

		if (m_mouseDown)
		{
			float depth = ( m_openNiHandler->GetNormalCalculator().GetDepthAtPoint(m_mousePos.x, m_mousePos.y));
			math::vector3d norm = m_openNiHandler->GetNormalCalculator().GetNormalAtPoint(m_mousePos.x, m_mousePos.y);
			math::vector4d depthNorm;
			depthNorm.set(norm.x, norm.y, norm.z, depth);
			core::string msg = mT("Depth Norm:") + core::StringConverter::toString(depthNorm);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
		}

		m_guiRenderer->Flush();
	}
	return rt;
}

void GeomDepthState::Update(float dt)
{
	IRenderingState::Update(dt);
	m_openNiHandler->Update(dt);
}


void GeomDepthState::LoadFromXML(xml::XMLElement* e)
{
}

}
}