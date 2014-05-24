	

#include "stdafx.h"

#include "LoginScreenState.h"
#include "ATAppGlobal.h"
#include "IVideoDevice.h"
#include "GUILoginMenu.h"
#include "GUIOverlay.h"
#include "GUIManager.h"
#include "GUIThemeManager.h"
#include "IGUIPanel.h"
#include "StringUtil.h"
#include "FontResourceManager.h"
#include "Application.h"
#include "GUILoadingElement.h"

namespace mray
{
namespace AugTel
{


LoginScreenState::LoginScreenState(const core::string& name)
	:IRenderingState(name)
{
	m_loginMenu=new GUILoginMenu();
	m_status = EIdle;

	IDelegateContainer::AddDelegate(CreateObjectDelegate(LoginScreenState,this,OnExitPressed));
	IDelegateContainer::AddDelegate(CreateObjectDelegate(LoginScreenState, this, OnConnectLocalPressed));
	IDelegateContainer::AddDelegate(CreateObjectDelegate(LoginScreenState, this, OnConnectRemotePressed));
	IDelegateContainer::AddDelegate(CreateObjectDelegate(LoginScreenState, this, OnDepthViewerPressed));
}
LoginScreenState::~LoginScreenState()
{
}

void LoginScreenState::OnConnectRemotePressed(IObject* caller, void* args)
{
	m_loginMenu->MessageLbl->SetText(core::string("Done!!"));
	m_exitCode = ToRemoteCamera_CODE;
	m_status = EDone;

	return;
}
void LoginScreenState::OnConnectLocalPressed(IObject* caller, void* args)
{
	m_loginMenu->MessageLbl->SetText(core::string("Done!!"));
	m_exitCode = ToLocalCamera_CODE;
	m_status = EDone;

	return; 
}
void LoginScreenState::OnDepthViewerPressed(IObject* caller, void* args)
{
	m_loginMenu->MessageLbl->SetText(core::string("Done!!"));
	m_exitCode = ToDepthView_CODE;
	m_status = EDone;
}

void LoginScreenState::OnExitPressed(IObject* caller, void* args)
{
	m_status = EDone;
	ATAppGlobal::Instance()->App->terminate();
}

void LoginScreenState::InitState()
{
	IRenderingState::InitState();

	m_guiManager=new GUI::GUIManager(Engine::getInstance().getDevice());
	m_guiManager->SetActiveTheme(GUI::GUIThemeManager::getInstance().getActiveTheme());

	m_guiroot=(GUI::IGUIPanelElement*)new GUI::IGUIPanelElement(core::string(""),m_guiManager);
	m_guiroot->SetDocking(GUI::EED_Fill);
	m_guiManager->SetRootElement(m_guiroot);

	GUI::GUILoadingElement* elem=new GUI::GUILoadingElement(m_guiManager);
	elem->SetPosition(math::vector2d(40,40));
	elem->SetSize(200);
	m_guiroot->AddElement(elem);

	GUI::GUIOverlay* o= GUI::GUIOverlayManager::getInstance().LoadOverlay("GUILoginMenu.gui");
	if(o)
	{
		o->CreateElements(m_guiManager,m_guiroot,this,m_loginMenu);
	}
}

bool LoginScreenState::OnEvent(Event* e,const math::rectf& rc)
{
	if(m_guiManager && m_status!=EDone)
	{
		return m_guiManager->OnEvent(e,&rc);
	}
	return false;
}

void LoginScreenState::OnEnter(IRenderingState*prev)
{
	m_status = EStatus::EIdle;
}
void LoginScreenState::OnExit()
{
	IRenderingState::OnExit();
}
video::IRenderTarget* LoginScreenState::Render(const math::rectf& rc, TBee::ETargetEye eye)
{
	IRenderingState::Render(rc,eye);


	video::IVideoDevice* dev=Engine::getInstance().getDevice();
	dev->setRenderTarget(m_renderTarget[GetEyeIndex(eye)]);
//	dev->set2DMode();
	Engine::getInstance().getDevice()->draw2DRectangle(rc,video::DefaultColors::Black);
	math::rectf vp(0, m_renderTarget[GetEyeIndex(eye)]->getSize());
	m_guiManager->DrawAll(&vp);

	GUI::GUIBatchRenderer renderer;
	renderer.SetDevice(Engine::getInstance().getDevice());
	GCPtr<GUI::IFont> font=gFontResourceManager.getDefaultFont();
	if(font){
		renderer.Prepare();

		float yoffset=50;


		GUI::FontAttributes attr;
		attr.fontColor.Set(1,1,1,1);
		attr.fontAligment=GUI::EFA_MiddleLeft;
		attr.fontSize=14;
		attr.hasShadow=true;
		attr.shadowColor.Set(0,0,0,1);
		attr.shadowOffset=math::vector2d(2);
		attr.spacing=2;
		attr.wrap=0;
		attr.RightToLeft=0;
		core::string msg=mT("Version= ");
		msg+=ATAppGlobal::Instance()->GetVersion();
		font->print(math::rectf(rc.BRPoint.x-300,rc.BRPoint.y-100+yoffset,10,10),&attr,0,msg,&renderer);
		yoffset+=attr.fontSize;
		msg=mT("Build= ");
		msg += ATAppGlobal::Instance()->GetBuild();
		font->print(math::rectf(rc.BRPoint.x-300,rc.BRPoint.y-100+yoffset,10,10),&attr,0,msg,&renderer);
		yoffset+=attr.fontSize;

	}
	renderer.Flush();
	dev->setRenderTarget(0);
	return m_renderTarget[GetEyeIndex(eye)];
}

void LoginScreenState::Update(float dt)
{
	m_guiManager->Update(dt);
}

void LoginScreenState::LoadFromXML(xml::XMLElement* e)
{
}

}
}
