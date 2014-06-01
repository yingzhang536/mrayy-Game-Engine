

#include "stdafx.h"
#include "SessionScene.h"

#include "GUIScreenLayoutImplV2.h"
#include "GUIManager.h"
#include "GUIThemeManager.h"
#include "GUIOverlayManager.h"
#include "GUIOverlay.h"

#include "ParsedShaderPP.h"
#include "ViewPort.h"
#include "SceneManager.h"

#include "SessionRenderer.h"
#include "AppData.h"
#include "TwitterProvider.h"
#include "TwitterTweet.h"

#include "TweetNode.h"
#include "SpeakerNode.h"
#include "IThreadFunction.h"
#include "IThreadManager.h"
#include "GUISceneSpacePanel.h"
#include "GUIElementRegion.h"

#include "GUISessionSidePanel.h"
#include "GUISpeakerDetailsPanel.h"
#include "PointerNode.h"
#include "Application.h"
#include "RenderWindow.h"

#include "FontResourceManager.h"
#include "IGUIRenderer.h"

namespace mray
{
namespace ted
{



	class SessionRendererCallback
	{
		GUI::GUISessionSidePanel* m_sidePanel;
		CSpeaker* m_selectedSpeaker;
	public:

		SessionRendererCallback()
		{
			m_selectedSpeaker = 0;
		}
		CSpeaker* GetSpeaker(){ return m_selectedSpeaker; }

		void SetSidePanel(GUI::GUISessionSidePanel* sp)
		{
			sp->OnSpeakerChange += CreateObjectDelegate(SessionRendererCallback, this, _OnSpeakerChange);
		}
		void _OnSpeakerChange(IObject* sender, PVOID param)
		{
			m_selectedSpeaker = (CSpeaker*)param;
			gAppData.SpeakerChange.__FIRE__OnSpeakerChange(m_selectedSpeaker);
		}

	}g_sessionCallback;

	class TwitterProviderListener :public ted::ITwitterProviderListener,public OS::IThreadFunction
	{
	protected:
		float m_lastTime;
		OS::IThreadPtr m_thread;
		ted::IDType m_sinceID;
	public:
		scene::SessionRenderer* r;
		float interval;
	public:

		TwitterProviderListener()
		{
			interval = 10000;
			m_sinceID = 0;
		}
		~TwitterProviderListener()
		{
			Stop();
		}

		void Start()
		{
			m_thread = OS::IThreadManager::getInstance().createThread(this);
			m_thread->start(0);
			m_lastTime = 0;

		}
		void Stop()
		{
			OS::IThreadManager::getInstance().killThread(m_thread);
			m_thread = 0;
		}
		virtual void OnTweetsLoaded(const std::vector<ted::TwitterTweet*>& tweets)
		{
			printf("Tweets Loaded: %d\n", tweets.size());
			std::vector<ted::TwitterTweet*> nodes;
			for (int i = 0; i < tweets.size(); ++i)
			{
				if (tweets[i]->ID>m_sinceID)
					m_sinceID = tweets[i]->ID;
				if (true||tweets[i]->text.find(L"#TEDxTokyo") != -1)
				{
					nodes.push_back(tweets[i]);
				}else
					delete tweets[i];
			}
			r->AddTweets(nodes);
		}
		virtual void execute(OS::IThread*caller, void*arg)
		{
			while (caller->isActive())
			{
				float t = gEngine.getTimer()->getSeconds();
				float dt = (t - m_lastTime);
				if (dt > interval)
				{
					std::vector<ted::TwitterTweet*> tweets;
					gAppData.tweetProvider->GetTweetsSynced(L"#TEDxTokyo ", m_sinceID, 150, tweets);
					OnTweetsLoaded(tweets);
					m_lastTime = gEngine.getTimer()->getSeconds();
				}
			}
		}

	};

SessionScene::SessionScene()
{
	m_providerListener = new TwitterProviderListener;
	m_pointer = new scene::PointerNode();

	m_lastHandAppearTime = 0;

	m_tapped = 0;
	m_grasping = false;
}

SessionScene::~SessionScene()
{
	gAppData.leapDevice->RemoveListener(this);
	delete m_providerListener;
	delete m_pointer;
}



void SessionScene::Init()
{
	IRenderingScene::Init();
	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	m_guiMngr = new GUI::GUIManager(dev);
	m_sceneManager = new scene::SceneManager(dev);

	{
		video::IRenderTargetPtr rt = dev->createRenderTarget("", dev->createTexture2D(1, video::EPixel_R8G8B8A8, false), 0, 0, 0);
		m_vp = new scene::ViewPort("", m_camera, rt, 0, math::rectf(0, 0, 1, 1), 0);
		m_vp->enablePostProcessing(true);

		video::ParsedShaderPP* pp = new video::ParsedShaderPP(dev);

		pp->LoadXML(gFileSystem.openFile("blur.peff"));
		m_vp->setPostProcessing(pp);
	}

	m_guiMngr->SetActiveTheme(GUI::GUIThemeManager::getInstance().getActiveTheme());

	m_guiroot = (GUI::IGUIPanelElement*)new GUI::IGUIPanelElement(core::string(""), m_guiMngr);
	m_guiroot->SetDocking(GUI::EED_Fill);
	m_guiMngr->SetRootElement(m_guiroot);

	{
		m_sessionRenderer = new scene::SessionRenderer();
		m_sessionRenderer->SetSessions(gAppData.sessions);

		//user loaded tweets

		m_sessionRenderer->AddTweets(TwitterTweet::TwitterTweetList);
	}
	{
		GUI::GUIOverlay* screenOverlay = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIScreenLayout_V2.gui");
		m_screenLayout = new GUI::GUIScreenLayoutImplV2();
		screenOverlay->CreateElements(m_guiMngr, m_guiroot, 0, m_screenLayout);
		g_sessionCallback.SetSidePanel(m_screenLayout->SessionsBar);
		m_screenLayout->ScenePanel->SetSessionRenderer(m_sessionRenderer);


		m_screenLayout->SessionsBar->SetSessionContainer(gAppData.sessions);
	}
	{
		m_bgm = gAppData.soundManager->loadSound("sounds//TedXTokyoBGM.mp3",true,sound::ESNDT_2D);
		m_bgm->setLooping(true);
	}
	if (true)
	{
		m_providerListener->r = m_sessionRenderer;
		
		if (gAppData.tweetProvider->IsAuthorized())
		{
			m_providerListener->Start();
		}


	}
}

void SessionScene::OnEnter()
{
	gAppData.leapDevice->AddListener(this);
	m_bgm->play();
}

void SessionScene::OnExit()
{
	gAppData.leapDevice->RemoveListener(this);
	m_bgm->stop();
}


bool SessionScene::OnEvent(Event* e, const math::rectf& rc)
{

	if (e->getType() == ET_Mouse && false)
	{
		MouseEvent *evt = (MouseEvent *)e;
		if (evt->event == MET_MOVED)
		{
			m_pointer->MoveBy(evt->pos - m_pointer->GetPosition());
			m_pointer->Show();
		}
		if (evt->event == MET_LEFTDOWN)
		{
			scene::ITedNode* node = m_sessionRenderer->GetNodeFromPosition(evt->pos);
			if (dynamic_cast<scene::TweetNode*>(node))
			{
				scene::TweetNode*tn=dynamic_cast<scene::TweetNode*>(node);
				ted::TwitterTweet* t = tn->GetTweet();
				m_screenLayout->TweetDetails->SetTweet(t);
				if (tn->GetSubTweets().size()>0)
					m_sessionRenderer->SetHoverdItem(node);
			}
			else 
			{
				m_sessionRenderer->SetHoverdItem(0);
			}
		}
	}
	else if (e->getType() == ET_Keyboard)
	{
		return m_guiMngr->OnEvent(e,&rc);
	}


	return false;
}

void SessionScene::Update(float dt)
{
	m_sceneManager->update(dt);
	m_guiMngr->Update(dt);
	m_sessionRenderer->Update(dt);

	m_pointer->Update(dt);

	{

		if ( m_screenLayout->SessionsBar->IsActive() )
		{
			m_screenLayout->GestureType->SetGesture(EGestureType::PushPull);
			if (m_grasping)
			{
				m_leapVec.x =  0;
				if (abs(m_leapVec.y) > 350)
				{
					if (!m_tapped)
					{
						m_tapped = true;
						int id = m_screenLayout->SessionsBar->GetCurrentSpeakerID();
						if (m_leapVec.y > 0)
						{
							id -= 1;
						}
						else
							id += 1;
						if (id >= 0 && id < m_screenLayout->SessionsBar->GetSpeakersCount())
							m_screenLayout->SessionsBar->SetCurrentSpeaker(id);
					}
				}
				else if (abs(m_leapVec.y)<20)
					m_tapped = false;
			}
			else 
				m_tapped = false;
		}
		else
		{
			m_screenLayout->GestureType->SetGesture(EGestureType::Swipe);
			m_tapped = false;
		}
	}
	{
		math::vector2d p(m_leapVec.x, m_leapVec.z);
		p *= 50;
		p += m_pointer->GetPosition();
		p.x = math::clamp<float>(p.x, 0.0f, gAppData.app->GetRenderWindow()->GetSize().x);
		p.y = math::clamp<float>(p.y, 0.0f, gAppData.app->GetRenderWindow()->GetSize().y);
		m_pointer->MoveBy(p - m_pointer->GetPosition());
		//m_pointer->Show();

		m_leapVec -= m_leapVec*math::Max(1.0f, dt)*0.1f;
		m_leapAcc = (m_leapVec - m_lastSpeedVec) / dt;
		m_lastSpeedVec = m_leapVec;
	}
	{
		MouseEvent e;
		math::rectf rc = m_vp->getAbsRenderingViewPort();
		e.event = MET_MOVED;
		e.pos = m_pointer->GetPosition();
		e.vpSize = rc.getSize();
		m_guiMngr->OnEvent(&e, &rc);

		scene::ITedNode* node = m_sessionRenderer->GetNodeFromPosition(e.pos);
		if (dynamic_cast<scene::TweetNode*>(node))
		{
			scene::TweetNode*tn = dynamic_cast<scene::TweetNode*>(node);
			ted::TwitterTweet* t = tn->GetTweet();
			m_screenLayout->TweetDetails->SetTweet(t);
		}
		m_sessionRenderer->SetHoverdItem(node);
		m_sessionRenderer->OnPointerMoved(e.pos);
	}
	{
		scene::SpeakerNode* node= m_sessionRenderer->GetCurrentSpeaker();
		if (node)
		{
			int cnt=node->GetSubTweets().size();
			if (cnt > 20)cnt = 20;
			float a = (1.0f-(float)cnt / 20.0f)*0.8+0.2f;
			m_screenLayout->ScenePanel->Info->SetTargetAlpha(a);
		}
	}

}

video::IRenderTarget* SessionScene::Draw(const math::rectf& rc)
{
	IRenderingScene::Draw(rc);

	Engine::getInstance().getDevice()->setRenderTarget(GetRenderTarget());
	m_guiMngr->DrawAll(&rc);
	m_pointer->Draw();


	GUI::IFont* font = gFontResourceManager.getDefaultFont();

	if (gAppData.Debugging && font)
	{
		GUI::FontAttributes attr;
		attr.fontColor.Set(1, 1, 1, 1);
		attr.fontAligment = GUI::EFA_MiddleLeft;
		attr.fontSize = 20;
		attr.hasShadow = false;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;
		core::UTFString msg;
		msg = mT("motion vec: ") + core::StringConverter::toString(m_leapVec);
		font->print(math::rectf(0, 400, 200, 100), &attr, 0, msg, m_guiMngr->GetRenderQueue());
		msg = mT("Grasping: ") + core::string(m_grasping? "Yes": "No");
		font->print(math::rectf(0, 430, 200, 100), &attr, 0, msg, m_guiMngr->GetRenderQueue());

		m_guiMngr->GetRenderQueue()->Flush();/**/
	}
	return GetRenderTarget();
}

void SessionScene::OnGesture(nui::LeapDevice* d, nui::ELeapGestureType type, Leap::Gesture g)
{
	return;
	if (type == nui::ELeapGestureType::Swipe)
	{
		Leap::SwipeGesture swipe = g;
		if (swipe.state() == Leap::Gesture::STATE_START)
		{
			//m_tapped = false;
		}
		if (swipe.state() == Leap::Gesture::STATE_UPDATE)
		{
			float speed = swipe.speed();
			if (speed < 50)
				speed = 0;
			if (speed > 200)
				speed = 200;
			/*
			speed /= 100.0f;
			speed *= speed;
			speed *= 100;*/
			math::vector3d dir(swipe.direction().x, swipe.direction().y, swipe.direction().z);
			m_leapVec = dir*speed;

			if (abs(m_leapVec.y) > 80 && m_screenLayout->SessionsBar->IsActive() )
			{
				if (!m_tapped)
				{
					m_tapped = true;
					int id = m_screenLayout->SessionsBar->GetCurrentSpeakerID();
					if (m_leapVec.y > 0)
					{
						id -= 1;
					}
					else
						id += 1;
					if (id >= 0 && id < m_screenLayout->SessionsBar->GetSpeakersCount())
						m_screenLayout->SessionsBar->SetCurrentSpeaker(id);

					m_leapVec = 0;
				}
			}
		}
		if (swipe.state() == Leap::Gesture::STATE_STOP)
		{
			m_leapVec = 0;
		}
	}
}
void SessionScene::OnHands(nui::LeapDevice* d, Leap::HandList hands)
{
	bool found=false;
	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Leap::Hand& hand = *hl;
		if (hand.isRight())
		{
			m_grasping = true;
			for (int i = 0; i < hand.fingers().count(); ++i)
			{
				if (hand.fingers()[i].isExtended())
				{
					m_grasping = false;
					break;;
				}
			}
			Leap::Vector vel= hand.palmVelocity();
			math::vector3d v(vel.x, vel.y, vel.z);
			float len = v.Length();


			float speed = v.Length();
			if (speed == 0)speed = 1;
			v /= speed;
			float minSpeed = 100;
			float maxSpeed = 2000;
			speed -= minSpeed;
			if (speed < 0)
				speed = 0;
			if (speed > maxSpeed)
				speed = maxSpeed;
			speed /= maxSpeed;
			speed = pow(speed,2);
			speed *= maxSpeed;
			math::vector3d dir = v;
			m_leapVec = dir*speed;


			if (false)
			{
				if (abs(m_leapVec.y) > 150 && m_screenLayout->SessionsBar->IsActive())
				{
					if (!m_tapped)
					{
						m_tapped = true;
						int id = m_screenLayout->SessionsBar->GetCurrentSpeakerID();
						if (m_leapVec.y > 0)
						{
							id -= 1;
						}
						else
							id += 1;
						if (id >= 0 && id < m_screenLayout->SessionsBar->GetSpeakersCount())
							m_screenLayout->SessionsBar->SetCurrentSpeaker(id);

						m_leapVec = 0;
					}
				}
				else if (abs(m_leapVec.y) < 100)
					m_tapped = false;
			}

			found = true;
		}
	}

	if (!found)
	{
		if (gEngine.getTimer()->getSeconds() - m_lastHandAppearTime > 2000 && m_pointer->IsVisible())
		{
			m_pointer->Hide();
		}
		m_leapVec = 0;
	}
	else
	{
		m_pointer->Show();
		m_lastHandAppearTime = gEngine.getTimer()->getSeconds();
	}
}

}
}
