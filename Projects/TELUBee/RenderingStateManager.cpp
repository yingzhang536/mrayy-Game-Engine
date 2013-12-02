
#include "stdafx.h"
#include "RenderingStateManager.h"
#include "XMLTree.h"
#include "StateMachine.h"
#include <IThreadManager.h>

#include "BaseRenderState.h"
#include "Application.h"
#include "TBAppGlobals.h"

#include "IntroRenderingState.h"
#include "LoginScreenState.h"
#include "InMapRenderState.h"
#include "NullRenderState.h"
#include "OculusDetectState.h"

#include "RenderBlender.h"
#include "ConnectingToRobotScreen.h"
#include "CameraRenderingState.h"
#include "GStreamerTestState.h"


namespace mray
{

namespace TBee
{

	class COnDoneCondition:public ICondition
	{
	protected:
		int m_code;
		StateMachine* m_machine;
	public:
		COnDoneCondition(StateMachine*m,const core::string&name,int code):ICondition(name)
		{
			m_machine=m;
			m_code=code;
		}
		virtual ~COnDoneCondition(){

		}
		virtual bool checkCondition()
		{
			BaseRenderState* s=(BaseRenderState*)m_machine->getActiveState();
			if(s && s->GetState())
				return s->GetState()->GetExitCode()==m_code;
			return false;
		}
	};


RenderingStateManager::RenderingStateManager(Application* app)
{
	m_oldState=0;
	m_stateMachine=new StateMachine();
	m_stateMachine->AddListener(this);
	m_app=app;

	m_blender=new RenderBlender();

	m_blender->LoadSettings("RenderStateManager");


	m_introState=new TBee::NullRenderState();
	m_introState->InitState(TBAppGlobals::App);
	AddState(m_introState,"Null");

	TBee::GStreamerTestState* streamerTest = new TBee::GStreamerTestState();
	streamerTest->InitState(TBAppGlobals::App);
	AddState(streamerTest, "Streamer");

	m_introState=new TBee::IntroRenderingState();
	m_introState->InitState(TBAppGlobals::App);
	AddState(m_introState,"Intro");

	TBee::OculusDetectState* oc= TBee::OculusDetectState::Instance();
	oc->InitState(TBAppGlobals::App);
	AddState(oc,"Oculus");

	TBee::LoginScreenState* ls=new TBee::LoginScreenState();
	m_loginState=ls;
	ls->InitState(TBAppGlobals::App);
	AddState(ls,"Login");

	TBee::InMapRenderState* maps=new TBee::InMapRenderState();
	maps->InitState(TBAppGlobals::App);
	AddState(maps,"Map");

	TBee::ConnectingToRobotScreen* ctr=new TBee::ConnectingToRobotScreen();
	ctr->InitState(TBAppGlobals::App);
	AddState(ctr,"Connecting");

	m_cameraState=new TBee::CameraRenderingState();
	m_cameraState->InitState(TBAppGlobals::App);
	AddState(m_cameraState,"Camera");


	AddTransition("Null","Streamer",STATE_EXIT_CODE);
	AddTransition("Streamer","Intro",STATE_EXIT_CODE);
	if(TBAppGlobals::usingOculus)
	{
		AddTransition("Intro","Oculus",STATE_EXIT_CODE);
		AddTransition("Oculus","Login",STATE_EXIT_CODE);
	}else
		AddTransition("Intro","Login",STATE_EXIT_CODE);
	AddTransition("Login","Map",ToMap_CODE);
	AddTransition("Login", "Camera", ToCamera_CODE);
	AddTransition("Camera", "Login", STATE_EXIT_CODE);
	AddTransition("Map","Login",BackToTile_Code);
	AddTransition("Map","Connecting",ConnectToRobot_Code);
	AddTransition("Connecting","Map",BackToMap_Code);
	SetInitialState("Null");
}
RenderingStateManager::~RenderingStateManager()
{
	delete m_stateMachine;
}

void RenderingStateManager::OnStateChanged(StateMachine*,IState* oldS,IState* newS)
{
	if(m_oldState)
		m_oldState->OnExit();
	m_oldState=((BaseRenderState*) oldS)->GetState();
	m_blender->Reset();
}

void RenderingStateManager::AddState(IRenderingState* st,const core::string&name)
{
	if(!st)
		return;
	m_stateMachine->addState(new BaseRenderState(name,st));

}

IRenderingState* RenderingStateManager::GetActiveState()
{
	BaseRenderState* s=(BaseRenderState*) m_stateMachine->getActiveState();
	if(!s)
		return 0;

	return s->GetState();
}

void RenderingStateManager::OnEvent(Event* event)
{
	BaseRenderState* s=(BaseRenderState*)m_stateMachine->getActiveState();
	if(!s)
		return ;

	if(event->getType()==ET_Keyboard)
	{
		KeyboardEvent* e=(KeyboardEvent*)event;
		if(e->press)
		{
			if(e->key==KEY_F8)
			{
				s->GetState()->ForceExit();
			}
		}
	}

	s->GetState()->OnEvent(event);

}
void RenderingStateManager::OnEnter(IApplicationState* prev)
{
	m_blender->Reset();
	m_oldState=0;
	BaseRenderState* s=(BaseRenderState*)m_stateMachine->getActiveState();
	if(s)
		s->GetState()->OnEnter(0);
}
void RenderingStateManager::OnExit()
{
	if(m_oldState)
	{
		m_oldState->OnExit();
		m_oldState=0;
	}
	BaseRenderState* s=(BaseRenderState*)m_stateMachine->getActiveState();
	if(s)
		s->GetState()->OnExit();

	IApplicationState::OnExit();
}
void RenderingStateManager::OnUpdate(float dt)
{
	BaseRenderState* s=(BaseRenderState*)m_stateMachine->getActiveState();
	if(s)
		s->GetState()->Update(dt);
	if(m_oldState)
	{
		m_oldState->Update(dt);
	}
	m_blender->Update(dt);
	if(m_oldState && m_blender->DoneBlending())
	{
		m_oldState->OnExit();
		m_oldState=0;
	}

	if(s)
	{
		bool canSleep=s->GetState()->CanSleep();
		if(m_oldState)
			canSleep&=m_oldState->CanSleep();

		if(canSleep)
		{
			float sleep=(1.0f/60.0f - dt)*1000;
			if(sleep>0)
			{
				OS::IThreadManager::getInstance().sleep(sleep);
			}
		}
	}

	m_stateMachine->onUpdate();


}
void RenderingStateManager::OnDraw(const math::rectf& rc,video::IRenderTarget* rt,ETargetEye eye)
{
	BaseRenderState* s=(BaseRenderState*)m_stateMachine->getActiveState();
	if(!s)
		return;

	video::TextureUnit t;

	if(m_oldState)
		m_oldState->Render(rc,eye);
	s->GetState()->Render(rc,eye);
	video::IRenderTarget* vrt=s->GetState()->GetLastFrame(eye);
	if(vrt)
		t.SetTexture(vrt->getColorTexture());

	math::vector2d r=rc.getSize();
	if(rt)
	{
		m_app->getDevice()->setRenderTarget(rt,true,true,video::SColor(1,1,1,0));
		r=rt->getSize();
	}
	if(m_oldState && m_oldState->GetLastFrame(eye))
	{
		m_blender->BlendImages(m_oldState->GetLastFrame(eye)->getColorTexture(),t.GetTexture(),math::rectf(0,r));

	}else
	{
		m_blender->BlendImages(0,t.GetTexture(),math::rectf(0,r));
	}
	if(rt)
		m_app->getDevice()->setRenderTarget(0);

}

void RenderingStateManager::SetInitialState(const core::string&name)
{
	m_stateMachine->setActiveState(name);
}
void RenderingStateManager::AddTransition(const core::string&a,const core::string&b,int code)
{
	core::string name=mT("Trans@")+a+"_"+b+"#";
	name+=core::StringConverter::toString(code);
	COnDoneCondition*cond= new COnDoneCondition(m_stateMachine,name,code);
	if(!m_stateMachine->addCondition(cond))
		delete cond;
	m_stateMachine->addTransition(a,b,name);
}


void RenderingStateManager::LoadSettingsXML(xml::XMLElement* e)
{
	xml::XMLAttribute*attr=0;
	xml::XMLElement* se=e->getSubElement("Intro");
	if(se)
		m_introState->LoadFromXML(se);
	se=e->getSubElement("Loading");
	if(se)
		m_loadingState->LoadFromXML(se);
	se=e->getSubElement("Login");
	if(se)
	{
		m_loginState->LoadFromXML(se);
	}
	se=e->getSubElement("Camera");
	if(se)
	{
		m_cameraState->LoadFromXML(se);
	}
}

}
}


