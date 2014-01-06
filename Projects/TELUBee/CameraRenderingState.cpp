

#include "stdafx.h"
#include "CameraRenderingState.h"


#include "DirectShowVideoGrabber.h"
#include "TextureResourceManager.h"

#include "JoystickEvent.h"
#include "JoystickDefinitions.h"

#include "Application.h"
#include "TBAppGlobals.h"
#include <IThreadManager.h>

#include "RemoteRobotCommunicator.h"
#include "TBAppGlobals.h"
#include "OculusDevice.h"
#include "InputManager.h"
#include "GUIBatchRenderer.h"
#include "FontResourceManager.h"

namespace mray
{
namespace TBee
{
	int s_id=0;
	typedef video::DirectShowVideoGrabber VCameraType ;

CameraRenderingState::CameraRenderingState()
{
	m_exitCode=0;

	m_VerticalShift=0;

	m_cameraSource[0].id=1;
	m_cameraSource[1].id=2;
	m_eyes[1].cw = true;

	m_cameraSource[0].videoGrabber=new video::VideoGrabberTexture();
	m_cameraSource[1].videoGrabber=new video::VideoGrabberTexture();

	GUI::GUIBatchRenderer*r = new GUI::GUIBatchRenderer();
	r->SetDevice(Engine::getInstance().getDevice());
	m_guiRenderer = r;

	m_capturing = false;
	m_clickCount = 0;
}


CameraRenderingState::~CameraRenderingState()
{
	delete m_cameraSource[0].videoGrabber;
	delete m_cameraSource[1].videoGrabber;

	delete m_guiRenderer;
}
math::vector2d CameraRenderingState::_GetEyeResolution(int i)
{
	return m_cameraSource[i].camera->GetFrameSize();
}
video::ITexturePtr CameraRenderingState::GetEyeTexture(int i)
{
	return m_cameraSource[i].videoGrabber->GetTexture();
}

void CameraRenderingState::InitState(Application* app)
{
	IEyesRenderingBaseState::InitState(app);

	int c=1;
	if(TBAppGlobals::StereoMode!=scene::EStereo_None)
		c=2;
	for(int i=0;i<2;++i)
	{
		m_eyes[i].flip90 = true;
		m_cameraSource[i].camera=new VCameraType();
		video::ITexturePtr tex=app->getDevice()->createEmptyTexture2D(true);

		m_cameraSource[i].videoGrabber->Set(m_cameraSource[i].camera,tex);
	}
}

void CameraRenderingState::SetCameraInfo(ETargetEye eye,int id)
{
	int index=GetEyeIndex(eye);
	m_cameraSource[index].id=id;

	_UpdateCameraParams();
}




void CameraRenderingState::OnEvent(Event* e)
{
	IEyesRenderingBaseState::OnEvent(e);
	if(e->getType()==ET_Mouse)
	{
		MouseEvent* ev = (MouseEvent*)e;
		if(ev->event==MET_LEFTDOWN)
		{
			if (m_capturing)
			{
				if (m_clickCount == 0)
				{
					m_firstClick = ev->pos;
					m_clickCount++;
				}
				else if (m_clickCount == 1)
				{
					m_BoxSize = ev->pos - m_firstClick;
					m_BoxSize.x = abs(m_BoxSize.x);
					m_BoxSize.y = abs(m_BoxSize.y);
					m_capturing = false;
					m_clickCount++;
				}
			}
		}
	}
	if(e->getType()==ET_Keyboard)
	{
		KeyboardEvent* evt=(KeyboardEvent*)e;
		if (evt->press)
		{
			if(evt->key>=KEY_1 && evt->key<=KEY_9)
			{
				ETargetEye  eye=Eye_Left;
				if(evt->shift)
					eye=Eye_Right;
				int index=GetEyeIndex(eye);
				//change the camera
				CameraInfo &ifo= m_cameraSource[index];
				if(evt->key-KEY_1!=m_cameraSource[1-index].id)
				{
					ifo.id=evt->key-KEY_1;
					ifo.camera->Stop();
					ifo.camera->InitDevice(ifo.id,m_cameraResolution.x,m_cameraResolution.y,m_cameraFPS);
					_UpdateCameraParams();
				}
			}
			else if (evt->key == KEY_ESCAPE)
			{
				m_exitCode = STATE_EXIT_CODE;
			}
			else
			if (evt->key == KEY_B)
			{
				m_capturing = true;
				m_clickCount = 0;
			}
		}
	}
}


void CameraRenderingState::OnEnter(IRenderingState*prev)
{
	IEyesRenderingBaseState::OnEnter(prev);
	//VCameraType* cam=(VCameraType*)m_camera;//m_video->GetGrabber().pointer();
	m_cameraSource[0].camera->InitDevice(m_cameraSource[0].id, m_cameraResolution.x, m_cameraResolution.y, m_cameraFPS);
	if(m_cameraSource[1].camera)
		m_cameraSource[1].camera->InitDevice(m_cameraSource[1].id, m_cameraResolution.x, m_cameraResolution.y, m_cameraFPS);
	//cam->Start();
	//printf("Cam resolution=%dx%d@%d\n",m_camera->GetFrameSize().x,m_camera->GetFrameSize().y,m_camera->GetFrameRate());
	

}


void CameraRenderingState::OnExit()
{
	IRenderingState::OnExit();
	//VCameraType* cam=(VCameraType*)m_video->GetGrabber().pointer();
	m_cameraSource[0].camera->Stop();
	if(m_cameraSource[1].camera)
		m_cameraSource[1].camera->Stop();

}
video::IRenderTarget* CameraRenderingState::Render(const math::rectf& rc,ETargetEye eye)
{
	video::IRenderTarget*ret=IEyesRenderingBaseState::Render(rc, eye);
	//draw a grid
	video::IVideoDevice* dev = Engine::getInstance().getDevice();



	//_RenderUI(targetRect);
	dev->setRenderTarget(0);
	dev->useTexture(0,0);


	return ret;
}

void CameraRenderingState::Update(float dt)
{
	IEyesRenderingBaseState::Update(dt);

	m_cameraSource[0].videoGrabber->Blit();
	m_cameraSource[1].videoGrabber->Blit();
}


void CameraRenderingState::_RenderUI(const math::rectf& rc)
{
	GUI::IFont* font = gFontResourceManager.getDefaultFont();
	GUI::FontAttributes attr;
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	if (m_clickCount == 2)
	{
		int n = 20;
		for (int i = 0; i < n; ++i)
		{
			dev->draw2DLine(math::vector2d(0, i*m_BoxSize.y), math::vector2d(rc.getWidth(), i*m_BoxSize.y), video::SColor(0, 0, 0, 255));
			dev->draw2DLine(math::vector2d(i*m_BoxSize.x, 0), math::vector2d(i*m_BoxSize.x, rc.getHeight()), video::SColor(0, 0, 0, 255));
		}
	}
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

		controllers::IJoysticController* joystick = TBAppGlobals::inputMngr->getJoystick(0);
		math::rectf r = rc;

		if (joystick)
		{

			for (int i = 0; i < 4; ++i)
			{
				controllers::JoysticAxis a = joystick->getAxisState(i);
				core::string msg = mT("Axis")+core::StringConverter::toString(i)+"= "+ core::StringConverter::toString(a.abs);
				font->print(r, &attr, 0, msg, m_guiRenderer);
				r.ULPoint.y += attr.fontSize + 5;
			}


		}
		math::vector3d correctionX(m_correctionValue[0]->floatParam[0], m_correctionValue[0]->floatParam[1], m_correctionValue[0]->floatParam[2]);
		math::vector3d correctionY(m_correctionValue[1]->floatParam[0], m_correctionValue[1]->floatParam[1], m_correctionValue[1]->floatParam[2]);
		core::string msg = mT("Correction X:") + core::StringConverter::toString(correctionX);
		font->print(r, &attr, 0, msg, m_guiRenderer);
		r.ULPoint.y += attr.fontSize + 5;
		msg = mT("Correction Y:") + core::StringConverter::toString(correctionY);
		font->print(r, &attr, 0, msg, m_guiRenderer);
		r.ULPoint.y += attr.fontSize + 5;
		m_guiRenderer->Flush();
	}
}


void CameraRenderingState::LoadFromXML(xml::XMLElement* e)
{
	IEyesRenderingBaseState::LoadFromXML(e);
	xml::XMLAttribute* attr;


	attr=e->getAttribute("CameraLeft");
	if(attr)
		m_cameraSource[0].id=core::StringConverter::toInt(attr->value);
	attr=e->getAttribute("CameraRight");
	if(attr)
		m_cameraSource[1].id=core::StringConverter::toInt(attr->value);

}


}
}


