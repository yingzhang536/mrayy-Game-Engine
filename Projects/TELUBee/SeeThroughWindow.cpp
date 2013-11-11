

#include "stdafx.h"
#include "SeeThroughWindow.h"
#include "Application.h"
#include "OptionContainer.h"
#include "RenderWindow.h"
#include "IVideoDevice.h"
#include "HMDRobotRenderer.h"
#include "HMDCameraRenderer.h"
#include "IVideoProvider.h"
#include "TBeeOptiTrackObject.h"
#include "TBAppGlobals.h"
#include "IGUIPanelElement.h"
#include "GUIOverlayManager.h"
#include "GUIOverlay.h"
#include "EventQueue.h"
#include "KeyboardEvent.h"

#include "mrayOIS.h"
#include "CameraRenderingState.h"
#include "ICameraVideoGrabber.h"
#include "FontResourceManager.h"

namespace mray
{
namespace TBee
{

SeeThroughWindow::SeeThroughWindow()
{
	m_camera=0;
	m_robot=0;
	m_mode=None;
	m_guiManager=0;
	m_inputManager=0;
}
SeeThroughWindow::~SeeThroughWindow()
{
	Disconnect();
	delete m_camera;
	delete m_robot;
	delete m_guiManager;
}
/*
void SeeThroughWindow::OnVideoImage(BaseDataReceiver*r,video::ImageInfo* image,ECameraSource src)
{
	//printf("%d,%d\n",image->Size.x,image->Size.y);
	if(src==ECamera_Left)
	{
		m_remoteTex[0]->createTexture(math::vector3d(image->Size.x,image->Size.y,1),image->format);

		video::LockedPixelBox box(math::box3d(0,image->Size),image->format,image->imageData);
		m_remoteTex[0]->getSurface(0)->blitFromMemory(box);
	}else
	{
		m_remoteTex[1]->createTexture(math::vector3d(image->Size.x,image->Size.y,1),image->format);

		video::LockedPixelBox box(math::box3d(0,image->Size),image->format,image->imageData);
		m_remoteTex[1]->getSurface(0)->blitFromMemory(box);
	}
}
*/

void SeeThroughWindow::_SetCamera(ETargetEye eye,int id)
{

	HMDCameraInfo ifo=m_camera->GetCameraInfo(eye);
	ifo.id=id;
	m_camera->SetCameraInfo(eye,ifo);
	video::ICameraVideoGrabber* cam;
	cam=m_camera->GetCameraState()->GetCamera(eye);
	if(!cam)
		return;
	for(int i=0;i<m_cameraParams.size();++i)
	{

		cam->SetParameter(m_cameraParams[i].name,m_cameraParams[i].value);
	}
}
void SeeThroughWindow::Init(Application *app,const OptionContainer& appOptions)
{
	m_camera=new HMDCameraRenderer();
	m_robot=new HMDRobotRenderer();
	const SOptionElement* op= appOptions.GetOptionByName("Camera");
	if(op)
	{
		int i=op->getValueIndex();
		if(i>=0)
		{
			HMDCameraInfo ifo;
			ifo.id=i;
			ifo.w=1920;
			ifo.h=1080;
			ifo.fps=25;
			m_camera->SetCameraInfo(Eye_Left,ifo);
		}
	}
	m_camera->Init(app);
	m_camera->Enter();

	m_robot->Init(app);

	OptionContainer opt=appOptions;
	if(opt["Monitor"].value=="1")
		opt["Monitor"].value="0";
	else
		opt["Monitor"].value="1";
	m_2ndWnd= app->getDevice()->CreateRenderWindow(mT("Secondary"),app->GetRenderWindow()->GetSize(),app->GetRenderWindow()->IsFullScreen(),opt,0);
	app->AddRenderWindow(m_2ndWnd);
	m_vp=m_2ndWnd->CreateViewport(mT("HMD"),0,0,math::rectf(0,0,1,1),0);
	m_vp->AddListener(this);

	InputCreationPack pack(m_2ndWnd);
	m_inputManager=CreateOISInputManager(pack);


	m_eyes[0]=new scene::ViewPort("Left",0,0,m_2ndWnd,math::rectf(0,0,1,0.5),0);
	m_eyes[0]->SetAutoUpdateRTSize(true);
	m_eyes[1]=new scene::ViewPort("Right",0,0,m_2ndWnd,math::rectf(0,0.5,1,1),0);
	m_eyes[1]->SetAutoUpdateRTSize(true);
	
	SwitchToSeeThrough();

	
	{
		m_guiManager=new GUI::GUIManager(app->getDevice());

		GUI::IGUIPanelElement* root=new  GUI::IGUIPanelElement(core::string(""),m_guiManager);
		m_guiManager->SetRootElement(root);
		root->SetDocking(GUI::EED_Fill);

		GUI::GUIOverlay* o;
		/*
		o= GUI::GUIOverlayManager::getInstance().LoadOverlay("TBee\\GUIMapScreen.gui");
		if(o)
		{
		o->CreateElements(m_guiManager,root,0,0);
		}
		*/
		o= GUI::GUIOverlayManager::getInstance().LoadOverlay("TBee\\GUIRobotControls.gui");
		if(o)
		{
			o->CreateElements(m_guiManager,root,0,0);
		}
		/*o= GUI::GUIOverlayManager::getInstance().LoadOverlay("TBee\\GUISeethroughMessage.gui");
		if(o)
		{
			o->CreateElements(m_guiManager,root,0,0);
		}*/
		o= GUI::GUIOverlayManager::getInstance().LoadOverlay("TBee\\GUISeethroughScreen.gui");
		if(o)
		{
			o->CreateElements(m_guiManager,root,0,0);
		}
	}
//	ConnectToCameras("127.0.0.1",1212,1213);
//	SwitchToRemoteVision();

	std::set<uint> mask;
	mask.insert(ET_Keyboard.ID());
	EventQueue::getInstance().addEventHandler(this,mask);
	
}
void SeeThroughWindow::onEvent(Event* e)
{
	if(e->getType()==ET_Keyboard)
	{
		KeyboardEvent* evt=(KeyboardEvent*)e;
		if(evt->press && evt->key>=KEY_1 && evt->key<=KEY_9)
		{
			ETargetEye eye=Eye_Left;
			if(evt->shift)
				eye=Eye_Right;
			//change the camera
			HMDCameraInfo ifo= m_camera->GetCameraInfo(eye);
			ifo.id=evt->key-KEY_1;
			ifo.w=1920;
			ifo.h=1080;
			m_camera->SetCameraInfo(eye,ifo);
			if(m_mode==SeeThrough)
			{
				m_camera->Exit();
				m_camera->Enter();
			}
		}
	}
}


void SeeThroughWindow::onRenderDone(scene::ViewPort*vp)
{

	if(vp!=m_vp)
	{
		return;
	}
	if(m_mode!=None)
	{
		video::TextureUnit tc;
		Engine::getInstance().getDevice()->setScissorTest(false);
		IHMDRenderer* renderer;
		if(m_mode==SeeThrough)
		{
			m_camera->Grab(m_eyes[0]->getAbsViewPort(),Eye_Left);
			renderer=m_camera;
		}
		else
		{
			m_robot->Grab();
			renderer=m_robot;
			/*
			math::rectf trc=m_provider->GetTexRect(0);
			math::rectf rc=vp->getAbsViewPort();
			rc.BRPoint.x=rc.ULPoint.x+vp->getAbsViewPort().getWidth()/2;
			tc.SetTexture(m_remoteTex[1]);
			Engine::getInstance().getDevice()->useTexture(0,&tc);
			Engine::getInstance().getDevice()->draw2DImage(rc,1,0,&trc);

			trc=m_provider->GetTexRect(1);
			rc.ULPoint.x=rc.BRPoint.x;
			rc.BRPoint.x=vp->getAbsViewPort().BRPoint.x;
			tc.SetTexture(m_remoteTex[0]);
			Engine::getInstance().getDevice()->useTexture(0,&tc);
			Engine::getInstance().getDevice()->draw2DImage(rc,1,0,&trc);*/
		}
		//renderer->Render(m_eyes[0]->getAbsViewPort(),Left);
		//renderer->Render(m_eyes[1]->getAbsViewPort(),Right);
		renderer->Render(vp->getAbsViewPort(),Eye_Left);

		Engine::getInstance().getDevice()->setScissorTest(true);


		m_guiManager->DrawAll(vp);

		if(false)
		{
			GUI::FontAttributes attrs;
			attrs.fontSize=14;
			GUI::IFont* font=gFontResourceManager.getDefaultFont();

			core::string params[]=
			{

				video::ICameraVideoGrabber::Param_Brightness,
				video::ICameraVideoGrabber::Param_Contrast,
				video::ICameraVideoGrabber::Param_Hue,
				video::ICameraVideoGrabber::Param_Saturation,
				video::ICameraVideoGrabber::Param_Sharpness,
				video::ICameraVideoGrabber::Param_ColorEnable,
				video::ICameraVideoGrabber::Param_WhiteBalance,
				video::ICameraVideoGrabber::Param_Gain,
				video::ICameraVideoGrabber::Param_BacklightCompensation,

				video::ICameraVideoGrabber::Param_Pan,
				video::ICameraVideoGrabber::Param_Tilt,
				video::ICameraVideoGrabber::Param_Roll,
				video::ICameraVideoGrabber::Param_Zoom,
				video::ICameraVideoGrabber::Param_Exposure,
				video::ICameraVideoGrabber::Param_Iris,
				video::ICameraVideoGrabber::Param_Focus,
			};
			int n=sizeof(params)/sizeof(core::string);
			math::rectf r(50,100,400,100);
			video::ICameraVideoGrabber* c=m_camera->GetCameraState()->GetCamera(Eye_Left);
			for(int i=0;i<n;++i)
			{
				core::string v= c->GetParameter(params[i]);
				core::UTFString str= params[i]+core::string("=")+v;
				font->print(r,&attrs,0,str,m_guiManager->GetRenderQueue());
				r.ULPoint.y+=attrs.fontSize+5;
				r.BRPoint.y+=attrs.fontSize+5;
			}
			m_guiManager->GetRenderQueue()->Flush();
		}
	}
}


void SeeThroughWindow::ConnectToCameras(const core::string& ip,int port1,int port2)
{
	m_robot->GetProvider()->ConnectToCameras(ip,port1,port2);
}

void SeeThroughWindow::Disconnect()
{
	m_robot->GetProvider()->Disconnect();
	m_mode=None;
}

void SeeThroughWindow::SwitchToRemoteVision()
{
	if(m_mode==SeeThrough)
		m_camera->Exit();
	m_mode=RemoteVision;
}

bool SeeThroughWindow::IsConnected()
{
	return m_robot->GetProvider()->IsConnected();
}

void SeeThroughWindow::SwitchToSeeThrough()
{
	if(m_mode==RemoteVision)
		Disconnect();

	m_mode=SeeThrough;
	m_camera->Enter();
}

void SeeThroughWindow::Update(float dt)
{
	m_inputManager->capture();
	m_guiManager->Update(dt);
	if(m_mode==RemoteVision)
		m_robot->Update(dt);
	else if(m_mode==SeeThrough)
	{
		m_camera->Update(dt);

		m_lookAtScreenFactor=math::clamp<float>(abs(TBAppGlobals::optiObj->GetRotation().y)-20,0,30)/3.0f;

	}
}

void SeeThroughWindow::LoadFromXML(xml::XMLElement* e)
{
	xml::XMLElement* elem=e->getSubElement("Parameter");
	while(elem)
	{
		CameraParams p;
		p.name=elem->getValueString("Name");
		p.value=elem->getValueString("Value");
		m_cameraParams.push_back(p);
		elem=elem->nextSiblingElement("Parameter");
	}
}

}
}
