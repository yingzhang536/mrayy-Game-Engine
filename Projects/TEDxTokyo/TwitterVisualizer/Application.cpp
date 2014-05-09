
#include "stdafx.h"
#include "Application.h"

#include "win32NetInterface.h"

#include "GUIBatchRenderer.h"

#include "TextureResourceManager.h"
#include "DynamicFontGenerator.h"

#include "GUIThemeManager.h"

#include "InternetCacheManager.h"
#include "FontResourceManager.h"
#include "ImageSetResourceManager.h"
#include "XMLDBHandler.h"
#include "SQLDBHandler.h"
#include "GUIElementFactory.h"

#include "GUIUserProfile.h"
#include "GUITweetItem.h"
#include "GUISessionSidePanel.h"
#include "GUISessionDetailsTopPanel.h"
#include "GUITweetDetailsPanel.h"
#include "GUIProfilePicture.h"
#include "SessionScene.h"

#include "TwitterService.h"

#include "AppData.h"
#include "LeapDevice.h"
#include "TwitterProvider.h"
#include "SessionContainer.h"

#include "TweetsVisualizeScene.h"
#include "Viewport.h"
#include "TwitterTweet.h"
#include "TwitterUserProfile.h"

namespace mray
{
Application::Application()
{
	m_limitFps = true;
	m_limitFpsCount = 60;
}
Application::~Application()
{
	ted::IDBHandler::getInstance().Close();
	delete ted::IDBHandler::getInstancePtr();

}

void Application::onEvent(Event* event)
{
	math::rectf rc;
	if (m_mainVP)
		rc = m_mainVP->getAbsRenderingViewPort();
	if (m_scene)
		m_scene->OnEvent(event, rc);
}

class TwitterProviderListener:public ted::ITwitterProviderListener
{
public:
	virtual void OnTweetsLoaded(const std::vector<ted::TwitterTweet*>& tweets)
	{
		printf("Tweets Loaded: %d\n", tweets.size());
		for (int i = 0; i < tweets.size();++i)
		{
			wprintf(L"Tweet[%s]: %s\n", tweets[i]->user->displayName.c_str(), tweets[i]->text.c_str());
		}
	}
}g_cb;
void Application::init(const OptionContainer &extraOptions)
{
	CMRayApplication::init(extraOptions);

	{
		new ted::AppData;
		gAppData.Load("tedSettings.cfg");
		gAppData.tweetProvider = new ted::TwitterProvider();
		new ted::XMLDBHandler(gAppData.GetValue("DB", "Users"), gAppData.GetValue("DB", "Tweets"));

	}

	{
		gAppData.Debugging = extraOptions.GetOptionValue("Debugging")=="Yes";
		gAppData.sessions = new ted::SessionContainer();
		gAppData.sessions->LoadFromXML("Sessions.xml");
	}

	if (true)
	{
		if (gAppData.tweetProvider->IsAuthorized())
		{
			std::vector<ted::TwitterTweet*> tweets;
			gAppData.tweetProvider->GetTweetsAsynced(L"tedxtokyo", 0, 100, &g_cb);
			/*
			{
				Twitter::UserSearchOptions op;
				Twitter::UserSearchResult res;
				op.screenName = "tedxtokyo";
				service.SearchForUser(op, res);
			}{
				Twitter::TwitterSearchOptions op;
				op.Q = "#Syria";
				op.count = 50;
				Twitter::TweetSearchResult res;
				service.Search(op, res);
			}*/
		}


	}
	CMRayApplication::loadResourceFile(mT("Resources.stg"));

	network::createWin32Network();

	{
		//gImageSetResourceManager.loadImageSet(mT("VistaCG_Dark.imageset"));
		GCPtr<OS::IStream> themeStream = gFileSystem.createBinaryFileReader(mT("VistaCG_Dark.xml"));
		GUI::GUIThemeManager::getInstance().loadTheme(themeStream);
		GUI::GUIThemeManager::getInstance().setActiveTheme(mT("VistaCG_Dark"));
		gFontResourceManager.loadFontsFromDir(gFileSystem.getAppPath() + "..\\Data\\Fonts\\");

		gImageSetResourceManager.loadImageSet("TedxTokyo.imageset");

		GCPtr<GUI::DynamicFontGenerator> font = new GUI::DynamicFontGenerator();
		font->SetFontName(L"Arial");
		font->SetTextureSize(1024);
		font->SetFontResolution(24);
		font->Init();
		gFontResourceManager.addResource(font, "Default");
		gFontResourceManager.setDefaultFont(font);
	}
	
	gAppData.leapDevice = new nui::LeapDevice;

	scene::ViewPort* vp = GetRenderWindow()->CreateViewport("", 0, 0, math::rectf(0, 0, 1, 1), 0);


	{
		REGISTER_GUIElement_FACTORY(GUITweetItem);
		REGISTER_GUIElement_FACTORY(GUIUserProfile);
		REGISTER_GUIElement_FACTORY(GUISessionSidePanel);
		REGISTER_GUIElement_FACTORY(GUISessionDetailsTopPanel);
		REGISTER_GUIElement_FACTORY(GUITweetDetailsPanel);
		REGISTER_GUIElement_FACTORY(GUIProfilePicture);
	}

	m_mainVP = GetRenderWindow()->CreateViewport("MainVP", 0, 0, math::rectf(0, 0, 1, 1), 0);


	// 	m_userProfile->GetFontAttributes()->hasShadow = false;
//	m_userProfile->GetFontAttributes()->fontColor.Set(0, 0, 0, 1);
	if (true)
	{
		try
		{
			ted::IDBHandler::getInstance().LoadDB();

		}
		catch (SAException& e)
		{
			printf("%s\n", e.ErrText());
		}
	}

	m_scene = new ted::SessionScene();
	m_scene->Init();

	m_scene->OnEnter();
}

void Application::WindowPostRender(video::RenderWindow* wnd)
{
	video::TextureUnit tex;

	getDevice()->set2DMode();
	getDevice()->setViewport(m_mainVP);
	m_scene->Draw(m_mainVP->getAbsRenderingViewPort());
	getDevice()->setRenderTarget(0);



	getDevice()->set2DMode();

	tex.SetTexture(m_scene->GetRenderTarget()->getColorTexture());
	getDevice()->useTexture(0, &tex);
	getDevice()->draw2DImage(m_mainVP->getAbsRenderingViewPort(), 1);

	GUI::GUIBatchRenderer m_guiRender;
	m_guiRender.SetDevice(getDevice());

	m_guiRender.Prepare();

	float yoffset = 50;

	GUI::IFont* font = gFontResourceManager.getDefaultFont();

	if (gAppData.Debugging && font)
	{
		GUI::FontAttributes attr;
		attr.fontColor.Set(0.05, 1, 0.5, 1);
		attr.fontAligment = GUI::EFA_MiddleLeft;
		attr.fontSize = 24;
		attr.hasShadow = true;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;
		core::UTFString msg;
		msg = L"FPS: " + core::string_to_wchar(core::StringConverter::toString(gFPS.getFPS()));
		font->print(math::rectf(0, 0, 200, 100), &attr, &math::rectf(0, 0, 200, 100), msg, &m_guiRender);
		
		msg = "Draw Calls=" + core::StringConverter::toString(getDevice()->getBatchDrawnCount());
		font->print(math::rectf(50, 30, 150, 200), &attr, 0, msg, &m_guiRender);

		msg = "Draw Primitives=" + core::StringConverter::toString(getDevice()->getPrimitiveDrawnCount());
		font->print(math::rectf(50, 60, 150, 200), &attr, 0, msg, &m_guiRender);

		m_guiRender.Flush();/**/
	}
}
void Application::update(float dt)
{
	CMRayApplication::update(dt);
	m_scene->Update(dt);
}

}



