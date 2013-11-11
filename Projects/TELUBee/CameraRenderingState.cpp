

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

	class CRobotConnector
	{
		core::string m_robotIP;
		RemoteRobotCommunicator* m_communicator;
		bool m_connected;
		bool m_status;

		math::vector2d m_speed;
		math::vector3d m_head;
	public:
		CRobotConnector()
		{
			m_connected = false;
			m_status = false;
			m_communicator = new RemoteRobotCommunicator();
		}
		~CRobotConnector()
		{
			delete m_communicator;
		}
		bool IsRobotConnected()
		{
			return m_status;
		}
		void ConnectRobot()
		{
			if (m_connected)
				m_communicator->Disconnect();
			m_connected = m_communicator->Connect(m_robotIP, 20000);
			//	m_roboComm->Connect("127.0.0.1",3000);
			m_communicator->SetUserID("yamens");
			m_communicator->ConnectUser(true);
		}
		void DisconnectRobot()
		{
			EndUpdate();
			m_communicator->Disconnect();
			m_connected = false;
		}
		void StartUpdate()
		{
			if (!m_connected)
				return;
			m_status = true;

			m_communicator->ConnectRobot(true);
		}
		void EndUpdate()
		{
			m_status = false;
			m_communicator->ConnectRobot(false);
		}
		void LoadXML(xml::XMLElement* e)
		{

			xml::XMLAttribute*attr = e->getAttribute("IP");
			if (attr)
			{
				m_robotIP = attr->value;
			}
		}

		void HandleController()
		{
			controllers::IJoysticController* joystick= TBAppGlobals::inputMngr->getJoystick(0);
			if (!joystick)
			{
				m_speed = 0;
				return;
			}
			controllers::JoysticAxis x = joystick->getAxisState(0);
			controllers::JoysticAxis y = joystick->getAxisState(1);

			m_speed.x = x.abs;
			m_speed.y = y.abs;
		}
		void UpdateStatus()
		{
			if (!m_status || !m_connected)
				return;
			HandleController();
			TBAppGlobals::oculusDevice->GetOrientation().toEulerAngles(m_head);
			m_communicator->SetData("Head", core::StringConverter::toString(m_head));
			m_communicator->SetData("Speed", core::StringConverter::toString(m_speed));
		}

	};
CameraRenderingState::CameraRenderingState()
{
	m_exitCode=0;

	m_VerticalShift=0;

	m_cameraSource[0].id=1;
	m_cameraSource[1].id=2;

	m_cameraResolution.set(640, 480);
	m_cameraFov = 25;

	m_cameraSource[0].videoGrabber=new video::VideoGrabberTexture();
	m_cameraSource[1].videoGrabber=new video::VideoGrabberTexture();

	m_robotConnector = new CRobotConnector();

	GUI::GUIBatchRenderer*r = new GUI::GUIBatchRenderer();
	r->SetDevice(Engine::getInstance().getDevice());
	m_guiRenderer = r;
}


CameraRenderingState::~CameraRenderingState()
{
	delete m_cameraSource[0].videoGrabber;
	delete m_cameraSource[1].videoGrabber;

	delete m_robotConnector;
	delete m_guiRenderer;
}

void CameraRenderingState::_UpdateCameraParams()
{
	m_targetAspectRatio=m_hmdSize.x/m_hmdSize.y;
	for(int i=0;i<2;++i)
	{
		video::ICameraVideoGrabber*camera= m_cameraSource[i].camera;
		if(!camera)
			return;
		math::vector2d framesize= camera->GetFrameSize();
		float camRatio=framesize.x/framesize.y;
		m_cameraSource[i].cropping.set(framesize.x,framesize.x/camRatio);
		m_cameraSource[i].ratio=camRatio;

		float focal=1;//in meter
		float w1=2*focal*tan(math::toRad(m_hmdFov*0.5f));
		float w2=2*(focal-m_hmdDistance)*tan(math::toRad(m_cameraFov*0.5f));

		float ratio=w2/w1;
		m_cameraSource[i].scale=m_hmdSize*ratio;
	}
}

void CameraRenderingState::SetParameters(float targetAspectRatio,float hmdDistance,float cameraFov,float hmdFov)
{
	m_targetAspectRatio=targetAspectRatio;
	m_hmdDistance=hmdDistance;
	m_cameraFov=cameraFov;
	m_hmdFov=hmdFov;
	_UpdateCameraParams();
}

void CameraRenderingState::InitState(Application* app)
{
	IRenderingState::InitState(app);
	int c=1;
	if(TBAppGlobals::StereoMode!=scene::EStereo_None)
		c=2;
	for(int i=0;i<2;++i)
	{
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
	if(e->getType()==ET_Keyboard)
	{
		KeyboardEvent* ev=(KeyboardEvent*)e;
		if(ev->press && ev->key==KEY_UP)
		{
			m_VerticalShift+=1;
		}else if(ev->press && ev->key==KEY_DOWN)
		{
			m_VerticalShift-=1;
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
		}
	}
	if (e->getType() == ET_Joystick)
	{
		JoystickEvent* evt = (JoystickEvent*)e;
		if (evt->event == JET_BUTTON_PRESSED)
		{
			if (evt->button == JOYSTICK_SelectButton)
			{
				m_exitCode = STATE_EXIT_CODE;
				m_robotConnector->EndUpdate();
			}
			else if (evt->button == JOYSTICK_StartButton)
			{
				if (m_robotConnector->IsRobotConnected())
					m_robotConnector->EndUpdate();
				else
					m_robotConnector->StartUpdate();
			}
		}
	}
}


void CameraRenderingState::OnEnter(IRenderingState*prev)
{
	//VCameraType* cam=(VCameraType*)m_camera;//m_video->GetGrabber().pointer();
	m_cameraSource[0].camera->InitDevice(m_cameraSource[0].id, m_cameraResolution.x, m_cameraResolution.y, m_cameraFPS);
	if(m_cameraSource[1].camera)
		m_cameraSource[1].camera->InitDevice(m_cameraSource[1].id, m_cameraResolution.x, m_cameraResolution.y, m_cameraFPS);
	//cam->Start();
	//printf("Cam resolution=%dx%d@%d\n",m_camera->GetFrameSize().x,m_camera->GetFrameSize().y,m_camera->GetFrameRate());

	_UpdateCameraParams();

	m_robotConnector->ConnectRobot();
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
	IRenderingState::Render(rc,eye);
	float shift=(float)m_VerticalShift*0.5f;
	int index=GetEyeIndex(eye);
	
	video::IVideoDevice* dev=Engine::getInstance().getDevice();
	dev->setRenderTarget(m_renderTarget[index]);

	m_cameraSource[index].videoGrabber->Blit();

//	gTextureResourceManager.writeResourceToDist(m_video->GetTexture(),"screens\\image#"+core::StringConverter::toString(s_id++)+".jp2");
	if (rc.getSize() != m_hmdSize)
	{
		m_hmdSize = rc.getSize();
		_UpdateCameraParams();
	}
	video::TextureUnit tex;
	tex.SetEdgeColor(video::DefaultColors::Black);
	tex.setTextureClamp(video::ETW_WrapS,video::ETC_CLAMP_TO_BORDER);
	tex.setTextureClamp(video::ETW_WrapT,video::ETC_CLAMP_TO_BORDER);
	tex.SetTexture(m_cameraSource[index].videoGrabber->GetTexture());
	dev->useTexture(0,&tex);
	//float croppingHeight=1-m_targetAspectRatio/m_cameraSource[index].ratio;
	float targetHeight = m_cameraSource[index].camera->GetFrameSize().x / m_targetAspectRatio;
	float heightDiff = targetHeight - m_cameraSource[index].camera->GetFrameSize().y;
	//float croppingHeight = 1-1.0f/(targetHeight / rc.getHeight());
	float croppingHeight = heightDiff / m_cameraSource[index].camera->GetFrameSize().y;
	croppingHeight*=0.5f;
	math::rectf tc(0,-croppingHeight,1,1+croppingHeight);
	//now calculate the actual rendering rectangle
	math::rectf targetRect;
	math::vector2d margin=(rc.getSize()-m_cameraSource[index].scale)/2;
	targetRect.ULPoint=rc.ULPoint+margin;
	targetRect.BRPoint=rc.BRPoint-margin;

	//tc.ULPoint=targetRect.ULPoint/rc.getSize();
	//tc.BRPoint=targetRect.BRPoint/rc.getSize();
	tc.ULPoint.y=1-tc.ULPoint.y;
	tc.BRPoint.y=1-tc.BRPoint.y;
	dev->draw2DImage(math::rectf(targetRect.ULPoint+math::vector2d(0,shift),targetRect.BRPoint+math::vector2d(0,shift)),1,0,&tc);

	//_RenderUI(targetRect);
	dev->setRenderTarget(0);
	dev->useTexture(0,0);


	return m_renderTarget[index].pointer();
}

void CameraRenderingState::Update(float dt)
{
	m_robotConnector->UpdateStatus();
}


void CameraRenderingState::_RenderUI(const math::rectf& rc)
{
	GUI::IFont* font = gFontResourceManager.getDefaultFont();
	GUI::FontAttributes attr;

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
		if (joystick)
		{
			math::rectf r=rc;

			for (int i = 0; i < 4; ++i)
			{
				controllers::JoysticAxis a = joystick->getAxisState(i);
				core::string msg = mT("Axis")+core::StringConverter::toString(i)+"= "+ core::StringConverter::toString(a.abs);
				font->print(r, &attr, 0, msg, m_guiRenderer);
				r.ULPoint.y += attr.fontSize + 5;
			}
			m_guiRenderer->Flush();

		}
	}
}


void CameraRenderingState::LoadFromXML(xml::XMLElement* e)
{
	IRenderingState::LoadFromXML(e);
	xml::XMLAttribute* attr;


	attr=e->getAttribute("CameraLeft");
	if(attr)
		m_cameraSource[0].id=core::StringConverter::toInt(attr->value);
	attr=e->getAttribute("CameraRight");
	if(attr)
		m_cameraSource[1].id=core::StringConverter::toInt(attr->value);

	attr=e->getAttribute("Size");
	if(attr)
	{
		m_cameraResolution=core::StringConverter::toVector2d(attr->value);


	}
	attr=e->getAttribute("FPS");
	if(attr)
	{
		m_cameraFPS=core::StringConverter::toInt(attr->value);
	}

	attr=e->getAttribute("CameraDistance");
	if(attr)
	{
		m_hmdDistance=core::StringConverter::toFloat(attr->value);
	}
	attr=e->getAttribute("CameraFov");
	if(attr)
	{
		m_cameraFov=core::StringConverter::toFloat(attr->value);
	}
	attr=e->getAttribute("HMDFov");
	if(attr)
	{
		m_hmdFov=core::StringConverter::toFloat(attr->value);
	}
	attr=e->getAttribute("HMDSize");
	if(attr)
	{
		m_hmdSize=core::StringConverter::toVector2d(attr->value);
	}

	m_robotConnector->LoadXML(e);
}


}
}


