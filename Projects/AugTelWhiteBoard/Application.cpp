

#include "stdafx.h"
#include "Application.h"
#include "ImageSetResourceManager.h"
#include "GUIThemeManager.h"
#include "FontResourceManager.h"
#include "win32NetInterface.h"
#include "OptiTrackDataSource.h"


namespace mray
{


Application::Application()
{
	m_optiProvider = new OptiTrackDataSource;
}

Application::~Application()
{
	delete m_optiProvider;
}

void Application::onEvent(Event* event)
{
	CMRayApplication::onEvent(event);
	m_lineDrawer.OnEvent(event);
}


void Application::init(const OptionContainer &extraOptions)
{
	CMRayApplication::init(extraOptions);
	{

		CMRayApplication::loadResourceFile(mT("Resources.stg"));


		gImageSetResourceManager.loadImageSet(mT("VistaCG_Dark.imageset"));
		GCPtr<OS::IStream> themeStream = gFileSystem.createBinaryFileReader(mT("VistaCG_Dark.xml"));
		GUI::GUIThemeManager::getInstance().loadTheme(themeStream);
		GUI::GUIThemeManager::getInstance().setActiveTheme(mT("VistaCG_Dark"));

		//load font
		GCPtr<GUI::IFont>font = gFontResourceManager.loadFont(mT("Calibrib_font.fnt"));
		gFontResourceManager.loadFont(mT("OCRAStd.fnt"));
		gFontResourceManager.setDefaultFont(font);

		gLogManager.log("Resources Loaded", ELL_SUCCESS);
	}
	m_mainVP = GetRenderWindow()->CreateViewport("MainVP", 0, 0, math::rectf(0, 0, 1, 1), 0);
	m_mainVP->SetClearColor(video::DefaultColors::White);
	network::createWin32Network();

	m_lineDrawer.StartReciver(5123);
	core::string optiIp=extraOptions.GetOptionValue("OptiServer");
	m_optiProvider->Connect(optiIp);

	if (!m_optiProvider->IsConnected())
		printf("Failed to connect with OptiTrack!\n");
}


void Application::draw(scene::ViewPort* vp)
{
}

void Application::WindowPostRender(video::RenderWindow* wnd)
{
	getDevice()->set2DMode();
	getDevice()->setViewport(m_mainVP);
	m_lineDrawer.Draw(m_mainVP->getAbsRenderingViewPort());
}

void Application::update(float dt)
{
	CMRayApplication::update(dt);
	m_lineDrawer.Update(dt);
}

void Application::onDone()
{
}


void Application::onRenderDone(scene::ViewPort*vp)
{
}

}

