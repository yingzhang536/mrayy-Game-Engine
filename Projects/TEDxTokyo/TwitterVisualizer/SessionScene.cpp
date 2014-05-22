

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
			interval = 5;
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
			std::vector<scene::TweetNode*> nodes;
			for (int i = 0; i < tweets.size(); ++i)
			{
				if (tweets[i]->ID>m_sinceID)
					m_sinceID = tweets[i]->ID;
				if (true||tweets[i]->text.find(L"#TEDxTokyo") != -1)
				{
					scene::TweetNode* n = new scene::TweetNode(0, tweets[i]);
					nodes.push_back(n);
				}else
					delete tweets[i];
			}
			r->AddTweetsNodes(nodes);
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
}

SessionScene::~SessionScene()
{
	delete m_providerListener;
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
	if (m_guiMngr->OnEvent(e, &rc))
		return true;

	if (e->getType() == ET_Mouse)
	{
		MouseEvent *evt = (MouseEvent *)e;
		if (evt->event == MET_MOVED)
		{
			scene::ITedNode* node = m_sessionRenderer->GetNodeFromPosition(evt->pos);
			if (dynamic_cast<scene::TweetNode*>(node))
			{
				scene::TweetNode*tn = dynamic_cast<scene::TweetNode*>(node);
				ted::TwitterTweet* t = tn->GetTweet();
				m_screenLayout->TweetDetails->SetTweet(t);
			}

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

	return false;
}

void SessionScene::Update(float dt)
{
	m_sceneManager->update(dt);
	m_guiMngr->Update(dt);
	m_sessionRenderer->Update(dt);

}

video::IRenderTarget* SessionScene::Draw(const math::rectf& rc)
{
	IRenderingScene::Draw(rc);

	Engine::getInstance().getDevice()->setRenderTarget(GetRenderTarget());
	m_guiMngr->DrawAll(&rc);
	return GetRenderTarget();
}


}
}
