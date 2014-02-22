
#include "stdafx.h"
#include "RenderingStateManager.h"
#include "XMLTree.h"
#include "StateMachine.h"
#include <IThreadManager.h>

#include "BaseRenderState.h"
#include "RenderBlender.h"
#include "AppData.h"


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


RenderingStateManager::RenderingStateManager()
{
	m_oldState=0;
	m_stateMachine=new StateMachine();
	m_stateMachine->AddListener(this);

	m_blender=new RenderBlender();

	m_blender->LoadSettings("RenderStateManager");


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

bool RenderingStateManager::OnEvent(Event* event, const math::rectf& rc)
{
	BaseRenderState* s=(BaseRenderState*)m_stateMachine->getActiveState();
	if(!s)
		return false;

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

	return s->GetState()->OnEvent(event,rc);

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

		if (AppData::Instance()->stereoMode == ERenderStereoMode::StereoTV)
			canSleep = false;

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
		Engine::getInstance().getDevice()->setRenderTarget(rt,true,true,video::SColor(1,1,1,1));
		r=rt->getSize();
	}
	if(m_oldState && m_oldState->GetLastFrame(eye))
	{
		m_blender->BlendImages(m_oldState->GetLastFrame(eye)->getColorTexture(),t.GetTexture(),math::rectf(0,r));

	}else
	{
		m_blender->BlendImages(0,t.GetTexture(),math::rectf(0,r));
	}
// 	if(rt)
// 		Engine::getInstance().getDevice()->setRenderTarget(0);

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

void RenderingStateManager::InitStates()
{

	const std::vector<IState*>& states = m_stateMachine->getStates();

	for (int i = 0; i < states.size(); ++i)
	{
		BaseRenderState* s = dynamic_cast<BaseRenderState*>(states[i]);
		if (s)
		{
			s->GetState()->InitState();
		}
	}
}


void RenderingStateManager::LoadSettingsXML(xml::XMLElement* e)
{
	xml::XMLAttribute*attr=0;

	const std::vector<IState*>& states= m_stateMachine->getStates();

	for (int i = 0; i < states.size(); ++i)
	{
		BaseRenderState* s = dynamic_cast<BaseRenderState*>(states[i]);
		if (s)
		{
			xml::XMLElement* se = e->getSubElement(s->getName());
			if (se)
				s->GetState()->LoadFromXML(se);
		}
	}
}

}
}


