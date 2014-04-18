
#include "stdafx.h"
#include "RenderingStateManager.h"
#include "XMLTree.h"
#include "StateMachine.h"
#include <IThreadManager.h>

#include "VideoclipRenderingState.h"
#include "BaseRenderState.h"
#include "Application.h"
#include "VTAppGlobals.h"

#include "VideoclipRenderingState.h"
#include "VTelesarRenderingState.h"
#include "CameraRenderingState.h"
#include "NullRenderState.h"


namespace mray
{

namespace VT
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

	core::string bs=VTAppGlobals::GetValue("RenderStateManager","BlendingSpeed");
	core::string staticB=VTAppGlobals::GetValue("RenderStateManager","StaticBlending");
	core::string BlendFactorA=VTAppGlobals::GetValue("RenderStateManager","BlendFactorA");
	core::string BlendFactorB=VTAppGlobals::GetValue("RenderStateManager","BlendFactorB");
	if(bs=="")
		SetBlendingSpeed(0.3);
	else
		SetBlendingSpeed(core::StringConverter::toFloat(bs));

	bool sb_=false;
	float bf1=0.5;
	float bf2=0.5;

	if(staticB!="")
		sb_=core::StringConverter::toBool(staticB);
	if(BlendFactorA!="")
		bf1=core::StringConverter::toFloat(BlendFactorA);
	if(BlendFactorB!="")
		bf2=core::StringConverter::toFloat(BlendFactorB);

	SetStaticBlending(sb_,bf1,bf2);


	VT::NullRenderState* ns=new VT::NullRenderState();
	VT::CameraRenderingState* cs=new VT::CameraRenderingState();
	cs->GetLeftCamera()->SetCameraInfo(0,640,480,60);
	cs->GetRightCamera()->SetCameraInfo(1,640,480,60);
	cs->InitState(VTAppGlobals::App);
	VT::VTelesarRenderingState* vs=new VT::VTelesarRenderingState();
	vs->InitState(VTAppGlobals::App);
	/*
	VT::VideoclipRenderingState* rs=new VT::VideoclipRenderingState();
	rs->SetFileName(false,"LOTR.ogv");
	rs->SetFileName(true,"LOTR.ogv");
	rs->InitState(VTAppGlobals::App);
	*/
	AddState(ns,"Null");
//	AddState(rs,"Video");
	AddState(cs,"Camera");
	AddState(vs,"Simulation");
	AddTransition("Null","Simulation",STATE_EXIT_CODE);
	AddTransition("Camera","Simulation",STATE_EXIT_CODE);
	AddTransition("Simulation","Camera",STATE_EXIT_CODE);
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
	m_blender.Reset();
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
			}else if(e->key==KEY_F5)
			{
				m_blender.SetStaticBlending(!m_blender.GetStaticBlending());
			}
		}
	}

	s->GetState()->OnEvent(event);

}
void RenderingStateManager::OnEnter(IApplicationState* prev)
{
	m_blender.Reset();
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
	if(s && s->GetState())
		s->GetState()->Update(dt);
	if(m_oldState)
	{
		m_oldState->Update(dt);
	}
	m_blender.Update(dt);
	if(m_oldState && m_blender.DoneBlending())
	{
		m_oldState->OnExit();
		m_oldState=0;
	}


	bool canSleep=true;
	if(s && s->GetState())
		canSleep=s->GetState()->CanSleep();
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

	m_stateMachine->onUpdate();


}
void RenderingStateManager::OnDraw(bool left,const math::rectf& rc,video::IRenderTarget* rt)
{
	BaseRenderState* s=(BaseRenderState*)m_stateMachine->getActiveState();
	if(!s)
		return;

	video::TextureUnit t;

	if(m_oldState)
		m_oldState->Render(left,rc);
	s->GetState()->Render(left,rc);
	video::IRenderTarget* vrt=s->GetState()->GetLastFrame(left);
	if(vrt)
		t.SetTexture(vrt->getColorTexture());

	math::vector2d r=rc.getSize();
	if(rt)
	{
		m_app->getDevice()->setRenderTarget(rt,true,true,video::SColor(1,1,1,0));
		r=rt->getSize();
	}
	if(m_oldState && m_oldState->GetLastFrame(left))
	{
		m_blender.BlendImages(m_oldState->GetLastFrame(left)->getColorTexture(),t.GetTexture(),math::rectf(0,r));

	}else
	{
		math::rectf tc(0,0,1,1);
		m_app->getDevice()->useTexture(0,&t);
		m_app->getDevice()->draw2DImage(rc,video::SColor(1,1,1,1),0,&tc);
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


void RenderingStateManager::LoadScenario(const core::string& fname)
{
	xml::XMLTree tree;
	if(!tree.load(fname))
		return;
	xml::XMLElement* e=tree.getSubElement("Scenarios");
	if(!e)
		return;

	xml::XMLAttribute*attr;
	xml::XMLElement* se=e->getSubElement("State");
	while(se)
	{
		IRenderingState* s=0;
		attr=se->getAttribute("Type");
		if(attr)
		{
			if(attr->value=="Videoclip")
			{
				s=new VideoclipRenderingState();
			}
		}

		if(s)
		{
			s->LoadFromXML(se);
		}

		se=se->nextSiblingElement("State");

	}
}
void RenderingStateManager::SetStaticBlending(bool e,float a,float b)
{
	m_blender.SetStaticBlending(e);
	m_blender.SetStaticBlendingParams(a,b);
	VTAppGlobals::SetValue("RenderStateManager","StaticBlending",core::StringConverter::toString(e));
	VTAppGlobals::SetValue("RenderStateManager","BlendFactorA",core::StringConverter::toString(a));
	VTAppGlobals::SetValue("RenderStateManager","BlendFactorB",core::StringConverter::toString(b));
}
void RenderingStateManager::SetBlendingSpeed(float s)
{
	m_blender.SetSpeed(s);
	VTAppGlobals::SetValue("RenderStateManager","BlendingSpeed",core::StringConverter::toString(s));
}

}
}


