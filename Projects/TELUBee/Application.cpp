



#include "stdafx.h"
#include "Application.h"
#include "FontResourceManager.h"
#include "ImageSetResourceManager.h"
#include "GUIThemeManager.h"
#include "StringUtil.h"

#include <TheoraManager.h>
#include <FSLManager.h>


#include "IThreadManager.h"
#include "RenderWindowUtils.h"
#include "SeeThroughWindow.h"

#include "TBAppGlobals.h"
#include "TextureResourceManager.h"
#include "win32NetInterface.h"
#include "mySqlManager.h"

#include "IndicatorComponent.h"
#include "GameComponentCreator.h"
#include "RobotInfoManager.h"

#include "XMLTree.h"

#include "GUIElementFactory.h"
#include "GUIDateElement.h"
#include "GUIBlinkingText.h"
#include "GUILoadingElement.h"

#include "TBeeOptiTrackObject.h"

#include "OculusManager.h"
#include "OculusDevice.h"
#include "OculusCameraComponent.h"
#include "ParsedShaderPP.h"

#include "OculusDetectState.h"


namespace mray
{
namespace TBee
{

	class ApplicationOculusData
	{
	public:
		ApplicationOculusData()
		{
			TBAppGlobals::oculusDevice = 0;

		}
		GCPtr<video::OculusManager> oculusManager;
		GCPtr<video::ParsedShaderPP> oculusRenderer[2];
		game::OculusCameraComponent* oculusComponents[2];

		video::ITexturePtr rtTexture;
		video::IRenderTargetPtr renderTarget;;

		~ApplicationOculusData()
		{
			oculusRenderer[0]=0;
			oculusRenderer[1]=0;
			TBAppGlobals::oculusDevice=0;
			oculusManager=0;
		}
	};


Application::Application()
{
	TBee::TBAppGlobals::App=this;
	m_drawUI=false;
	m_seeThroughWindow=0;

	m_oculusData=new ApplicationOculusData();
}
Application::~Application()
{
	delete m_oculusData;
	m_appStateManager=0;
	delete TBee::TBAppGlobals::sqlManager;
	m_videoManager=0;
	m_soundManager=0;
	delete TBAppGlobals::robotInfoManager;
	delete m_seeThroughWindow;
	delete TBAppGlobals::optiObj;
}

void Application::_InitResources()
{
	CMRayApplication::loadResourceFile(mT("Resources.stg"));


	gImageSetResourceManager.loadImageSet(mT("VistaCG_Dark.imageset"));
	GCPtr<OS::IStream> themeStream=gFileSystem.createBinaryFileReader(mT("VistaCG_Dark.xml"));
	GUI::GUIThemeManager::getInstance().loadTheme(themeStream);
	GUI::GUIThemeManager::getInstance().setActiveTheme(mT("VistaCG_Dark"));

	//load font
	GCPtr<GUI::IFont>font=gFontResourceManager.loadFont(mT("Calibrib_font.fnt"));
	gFontResourceManager.loadFont(mT("OCRAStd.fnt"));
	gFontResourceManager.setDefaultFont(font);

	gLogManager.log("Resources Loaded",ELL_SUCCESS);
}
void Application::onEvent(Event* event)
{
	CMRayApplication::onEvent(event);

	if(event->getType()==ET_Mouse)
	{
		MouseEvent* e=(MouseEvent* )event;
	}
	if(m_appStateManager)
		m_appStateManager->OnEvent(event);


	if(event->getType()==ET_Keyboard)
	{
		KeyboardEvent* e=(KeyboardEvent*)event;
		if(e->press && e->key==KEY_F12)
		{
			m_drawUI=!m_drawUI;
		}
		/*	if(e->press && e->key==KEY_F12)
		{

			video::LockedPixelBox box=m_screenShot->getSurface(0)->lock(math::box3d(0,0,0,GetRenderWindow()->GetSize().x,GetRenderWindow()->GetSize().y,1),video::IHardwareBuffer::ELO_Discard);
			GetRenderWindow()->TakeScreenShot(box);
			m_screenShot->getSurface(0)->unlock();
			gTextureResourceManager.writeResourceToDist(m_screenShot,"screenShot.tga");

		}*/
	}

}

void Application::init(const OptionContainer &extraOptions)
{
	CMRayApplication::init(extraOptions);

	TBee::TBAppGlobals::Load("TBSettings.conf");
	{
		core::string v=extraOptions.GetOptionValue("Debugging");
		if(v=="Yes")
			TBee::TBAppGlobals::IsDebugging=true;
		else
			TBee::TBAppGlobals::IsDebugging=false;
	
		TBee::TBAppGlobals::DVIPort=extraOptions.GetOptionValue("DVIPort");
	}
	_InitResources();

	LoadSettingsXML("TBeeSettings.xml");


	network::createWin32Network();


	REGISTER_COMPONENT_FACTORY(IndicatorComponent);
	using namespace GUI;
	REGISTER_GUIElement_FACTORY(GUIDateElement);
	REGISTER_GUIElement_FACTORY(GUILoadingElement);
	REGISTER_GUIElement_FACTORY(GUIBlinkingText);

	TBee::TBAppGlobals::sqlManager=new db::mySqlManager();

	m_guiRender=new GUI::GUIBatchRenderer();
	m_guiRender->SetDevice(getDevice());

	TBAppGlobals::inputMngr = m_inputManager;
// 	m_guiManager=new GUI::GUIManager(getDevice());
// 	m_guiManager->SetActiveTheme(GUI::GUIThemeManager::getInstance().getActiveTheme());

	m_soundManager=new sound::FSLManager();
	m_videoManager=new video::TheoraManager();
	m_videoManager->setSoundManager(m_soundManager);

	m_screenShot=getDevice()->createEmptyTexture2D(true);
	m_screenShot->setMipmapsFilter(false);
	m_screenShot->createTexture(math::vector3d(GetRenderWindow()->GetSize().x,GetRenderWindow()->GetSize().y,1),video::EPixel_B8G8R8A8);


	TBAppGlobals::robotInfoManager=new RobotInfoManager();

	m_appStateManager=new TBee::ApplicationStateManager();


//	TBee::ApplicationMenuState* menuState=new TBee::ApplicationMenuState();
	m_renderingState=new TBee::RenderingStateManager(this);
	//m_appStateManager->AddState(menuState,"Menu");
	m_appStateManager->AddState(m_renderingState,"Rendering");
	m_appStateManager->SetInitialState("Rendering");
	//m_appStateManager->AddTransition("Menu","Rendering",STATE_EXIT_CODE);
	{

		m_rtTexture=getDevice()->createEmptyTexture2D(true);
		m_rtTexture->setMipmapsFilter(false);

		m_renderTarget=getDevice()->createRenderTarget("",m_rtTexture,0,0,0);
	}

	TBAppGlobals::optiObj=new TBeeOptiTrackObject();
	TBAppGlobals::ConnectOpti();

	if(false)
	{
		m_seeThroughWindow=new SeeThroughWindow();
		TBAppGlobals::seeTrough=m_seeThroughWindow;
		m_seeThroughWindow->Init(this,extraOptions);
		GetRenderWindow(0)->SetActive(true);
	}

	// check for Oculus
	if(TBAppGlobals::usingOculus)
	{
		TBAppGlobals::StereoMode=scene::EStereo_SideBySide;
		//Create Oculus
		m_oculusData->oculusManager=new video::OculusManager();
		TBAppGlobals::oculusDevice=m_oculusData->oculusManager->CreateDevice(0);

		if(!TBAppGlobals::oculusDevice)
		{
			OculusDetectState::Instance()->SetState(GUIOculusDetectImpl::EFailedToConnect);
			//		gLogManager.log("Failed to detect OculusVR!",ELL_ERROR);
		}else
		{
			OculusDetectState::Instance()->SetState(GUIOculusDetectImpl::EFound);

			int w=TBAppGlobals::oculusDevice->GetDeviceInfo().deviceInfo.HResolution;
			int h=TBAppGlobals::oculusDevice->GetDeviceInfo().deviceInfo.VResolution;
			game::OculusCameraComponent::ETargetCamera cams[2]=
			{
				game::OculusCameraComponent::LeftCamera,
				game::OculusCameraComponent::RightCamera
			};
			m_viewPort[0]=GetRenderWindow()->CreateViewport("MainVPL",0,0,math::rectf(0,0,0.5,1),0);
			m_viewPort[0]->AddListener(this);
			m_viewPort[1]=GetRenderWindow()->CreateViewport("MainVPR",0,0,math::rectf(0.5,0,1,1),0);
			m_viewPort[1]->AddListener(this);
			//m_viewPort[1]->setActive(false);
			for(int i=0;i<2;++i)
			{
				m_oculusData->oculusRenderer[i]=new video::ParsedShaderPP(getDevice());
				m_oculusData->oculusRenderer[i]->LoadXML(gFileSystem.openFile("OculusLens.peff"));
				m_oculusData->oculusRenderer[i]->Setup(math::rectf(0,0,w/2,h));
				m_oculusData->oculusComponents[i]=new game::OculusCameraComponent(0);
				m_oculusData->oculusComponents[i]->InitCamera(TBAppGlobals::oculusDevice,m_viewPort[i],cams[i],m_oculusData->oculusRenderer[i]);

			}

			m_oculusData->rtTexture=getDevice()->createEmptyTexture2D(true);
			m_oculusData->rtTexture->setMipmapsFilter(false);
			m_oculusData->rtTexture->createTexture(math::vector3d(w/2,h,1),video::EPixel_R8G8B8);

			m_oculusData->renderTarget=getDevice()->createRenderTarget("",m_oculusData->rtTexture,0,0,0);
		}
	}else
	{

		m_viewPort[0]=GetRenderWindow()->CreateViewport("MainVP",0,0,math::rectf(0,0,1,1),0);
		m_viewPort[0]->AddListener(this);

	}
	LoadSettingsXML("States.xml");
}

void Application::RenderUI(scene::ViewPort* vp)
{
	if(m_drawUI )
	{


		GCPtr<GUI::IFont> font=gFontResourceManager.getDefaultFont();
		if(font){
			m_guiRender->Prepare();

			float yoffset=50;


			GUI::FontAttributes attr;
			attr.fontColor.Set(0.05,1,0.5,1);
			attr.fontAligment=GUI::EFA_MiddleLeft;
			attr.fontSize=24;
			attr.hasShadow=true;
			attr.shadowColor.Set(0,0,0,1);
			attr.shadowOffset=math::vector2d(2);
			attr.spacing=2;
			attr.wrap=0;
			attr.RightToLeft=0;
			core::string msg=mT("FPS= ");
			msg+=core::StringConverter::toString((int)core::CFPS::getInstance().getFPS());
			font->print(math::rectf(vp->getAbsRenderingViewPort().getWidth()-250,vp->getAbsRenderingViewPort().getHeight()-50,10,10),&attr,0,msg,m_guiRender);
			yoffset+=attr.fontSize;

		}

		m_guiRender->Flush();
		getDevice()->useShader(0);
	}
}

void Application::draw(scene::ViewPort* vp)
{


}
void Application::_RenderVP(int i)
{

	math::rectf rc=math::rectf(0,m_viewPort[i]->getAbsRenderingViewPort().getSize());

	if(m_rtTexture->getSize().x!=rc.getWidth() ||
		m_rtTexture->getSize().y!=rc.getHeight())
	{
		m_rtTexture->createTexture(math::vector3d(rc.getWidth(),rc.getHeight(),1),video::EPixel_R8G8B8A8);
	}

	m_appStateManager->Draw(rc,m_renderTarget,i==0?Eye_Left:Eye_Right);
	getDevice()->set2DMode();
	RenderUI(m_viewPort[i]);
	getDevice()->setRenderTarget(0,false,true,video::SColor(1,1,1,0));




	if(TBAppGlobals::usingOculus && TBAppGlobals::oculusDevice )
	{
		if(m_oculusData->oculusRenderer[i])
		{
			video::TextureUnit tex;
			getDevice()->set2DMode();
			{
				m_device->setRenderTarget(m_oculusData->renderTarget);
				math::matrix4x4 m,pm;
				getDevice()->getTransformationState(video::TS_PROJECTION,m);
				pm.f14=m_oculusData->oculusComponents[i]->GetPerspectiveOffset();
				pm=pm*m;
				getDevice()->setTransformationState(video::TS_PROJECTION,pm);

				tex.SetTexture(m_rtTexture);
				getDevice()->useTexture(0,&tex);
				math::rectf tc=math::rectf(0,1,1,0);
				getDevice()->draw2DImage(math::rectf(0,m_oculusData->renderTarget->getSize()),1,0,&tc);
				m_device->setRenderTarget(0);
				getDevice()->setTransformationState(video::TS_PROJECTION,m);
			}
			m_oculusData->oculusRenderer[i]->render(m_oculusData->renderTarget);
			tex.SetTexture(m_oculusData->oculusRenderer[i]->getOutput()->getColorTexture());
			getDevice()->useTexture(0,&tex);
			getDevice()->draw2DImage(rc,1);
		}
	}else
	{
		video::TextureUnit tex;

		tex.SetTexture(m_rtTexture);
		getDevice()->useTexture(0,&tex);
		getDevice()->draw2DImage(rc,1);
	}
}

void Application::onRenderDone(scene::ViewPort*vp)
{
	if(vp==m_viewPort[0])
	{
		_RenderVP(0);
	}
	if(vp==m_viewPort[1])
	{
		_RenderVP(1);
	}

}
void Application::WindowPostRender(video::RenderWindow* wnd)
{
}
void Application::update(float dt)
{
	CMRayApplication::update(dt);

	if(m_videoManager)
		m_videoManager->Update(dt);
	if(m_soundManager)
		m_soundManager->runSounds(dt);

	m_appStateManager->Update(dt);
	if(m_seeThroughWindow)
		m_seeThroughWindow->Update(dt);

	if(TBAppGlobals::usingOculus && TBAppGlobals::oculusDevice)
	{
		m_oculusData->oculusManager->Update(dt);
		TBAppGlobals::oculusDevice->Update(dt);

		m_oculusData->oculusComponents[0]->Update(dt);
		m_oculusData->oculusComponents[1]->Update(dt);
	}

	//OS::IThreadManager::getInstance().sleep(1000/30);

}
void Application::onDone()
{
	TBee::TBAppGlobals::Save("VTSettings.conf");

}

void Application::LoadSettingsXML(const core::string &path)
{
	xml::XMLTree tree;
	if(!tree.load(path))
		return;
	xml::XMLElement* e;
	e=tree.getSubElement("States");
	if(e)
	{
		xml::XMLElement* se=e->getSubElement("Rendering");
		if(se)
		{
			m_renderingState->LoadSettingsXML(se);
		}
		se=e->getSubElement("Camera");
		if(se && m_seeThroughWindow)
		{
			m_seeThroughWindow->LoadFromXML(se);
		}
	}
	e=tree.getSubElement("Application");
	if(e)
	{
		TBAppGlobals::usingOculus=e->getValueBool("UsingOculus");
	}
}

}
}
