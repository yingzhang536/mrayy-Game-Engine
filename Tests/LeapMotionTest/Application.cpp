



#include "stdafx.h"
#include "Application.h"
#include "FontResourceManager.h"
#include "ImageSetResourceManager.h"
#include "GUIThemeManager.h"
#include "StringUtil.h"


#include "IThreadManager.h"
#include "RenderWindowUtils.h"

#include "XMLTree.h"

#include "GUIElementFactory.h"

#include "ParsedShaderPP.h"
#include "DynamicFontGenerator.h"
#include "MeshFileCreatorManager.h"
#include "MaterialResourceManager.h"
#include "ShaderResourceManager.h"


namespace mray
{

Application::Application()
{
}
Application::~Application()
{
}

void Application::onEvent(Event* event)
{
	CMRayApplication::onEvent(event);

	if(event->getType()==ET_Mouse)
	{
		MouseEvent* e=(MouseEvent* )event;
	}


}



void Application::init(const OptionContainer &extraOptions)
{
	CMRayApplication::init(extraOptions);

	CMRayApplication::loadResourceFile(mT("Resources.stg"));


	m_sceneManager = new scene::SceneManager(Engine::getInstance().getDevice());

	gImageSetResourceManager.loadImageSet(mT("VistaCG_Dark.imageset"));
	GCPtr<OS::IStream> themeStream = gFileSystem.createBinaryFileReader(mT("VistaCG_Dark.xml"));
	GUI::GUIThemeManager::getInstance().loadTheme(themeStream);
	GUI::GUIThemeManager::getInstance().setActiveTheme(mT("VistaCG_Dark"));

	//load font

	GCPtr<GUI::DynamicFontGenerator> font = new GUI::DynamicFontGenerator("Arial");
	font->SetFontName(L"Arial");
	font->SetTextureSize(1024);
	font->SetFontResolution(24);
	font->Init();
	gFontResourceManager.setDefaultFont(font);
	this->m_limitFps = false;

	using namespace GUI;

	m_guiRender=new GUI::GUIBatchRenderer();
	m_guiRender->SetDevice(getDevice());

	m_oculusMngr = new video::OculusManager();
	printf("Devices connected:%d \n",m_oculusMngr->GetDeviceCount());
	m_oculusDevice = m_oculusMngr->CreateDevice(0);

	{
		scene::CameraNode* m_camera[2];
		m_headNode = m_sceneManager->createSceneNode();

		int w = GetRenderWindow()->GetSize().x;
		int h = GetRenderWindow()->GetSize().y;
		//m_viewPort[1]->setActive(false);
		for (int i = 0; i < 2; ++i)
		{
			m_camera[i] = m_sceneManager->createCamera();
			oculusRenderer[i] = new video::ParsedShaderPP(getDevice());
			oculusRenderer[i]->LoadXML(gFileSystem.openFile("OculusLens.peff"));
			oculusRenderer[i]->Setup(math::rectf(0, 0, w, h));


			m_camera[i]->setZNear(0.01);
			m_camera[i]->setZFar(100);

			//if (ATAppGlobal::Instance()->oculusDevice)
			m_camera[i]->setAutoUpdateAspect(true);
			m_camera[i] = m_camera[i];
		}
		m_camera[0]->setPosition(math::vector3d(-0.03, 0, 0));
		m_camera[1]->setPosition(math::vector3d(+0.03, 0, 0));

		scene::ISceneNode* head = m_sceneManager->createSceneNode();
		head->addChild(m_camera[0]);
		head->addChild(m_camera[1]);

		head->setPosition(math::vector3d(0, 0, -10));

		m_viewPort[0] = GetRenderWindow()->CreateViewport("MainVPL", m_camera[0], 0, math::rectf(0, 0, 0.5, 1), 0);
		m_viewPort[1] = GetRenderWindow()->CreateViewport("MainVPR", m_camera[1], 0, math::rectf(0.5, 0, 1, 1), 0);
		m_viewPort[0]->setPostProcessing(oculusRenderer[0]);
		m_viewPort[1]->setPostProcessing(oculusRenderer[1]);
		m_viewPort[0]->enablePostProcessing(true);
		m_viewPort[1]->enablePostProcessing(true);

		scene::LightNode* light= m_sceneManager->createLightNode("");
		light->setPosition(10);

		ShaderResourceManager::getInstance().parseShaderXML(gFileSystem.openFile("models\\AT_Shaders.shd"));
		MaterialResourceManager::getInstance().parseMaterialXML(gFileSystem.openFile("models\\AT_materials.mtrl"));
		scene::MeshRenderableNode* node= scene::MeshFileCreatorManager::getInstance().LoadFromFile("models\\SkinnedArms.mdx",false);

		m_sceneManager->createSceneNode("")->AttachNode(node);
	}
	{
	}

// 	script::PythonScriptManager* scriptManager = new script::PythonScriptManager();
// 	scriptManager->ExecuteFile(gFileSystem.openFile("testPython.py"));

}
void Application::draw(scene::ViewPort* vp)
{

	math::rectf rc = vp->getAbsRenderingViewPort();

	GCPtr<GUI::IFont> font = gFontResourceManager.getDefaultFont();
	if (font){
		m_guiRender->Prepare();

		float yoffset = 50;


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
		core::string msg = mT("FPS= ");
		msg += core::StringConverter::toString((int)gEngine.getFPS()->getFPS());
		font->print(math::rectf(rc.getWidth() - 250, rc.getHeight() - 50, 10, 10), &attr, 0, msg, m_guiRender);
		yoffset += attr.fontSize;

		if (m_oculusDevice)
		{
			math::vector3d pos;
			math::quaternion ori;
			m_oculusDevice->GetEyePos(video::OVREye::Left, pos, ori);

			msg = mT("Pos= ");
			msg += core::StringConverter::toString(pos);
			font->print(math::rectf(10, 20 + yoffset, 10, 10), &attr, 0, msg, m_guiRender);
			yoffset += attr.fontSize;

			msg = mT("Ori= ");
			math::vector3d angles;
			ori.toEulerAngles(angles);
			msg += core::StringConverter::toString(angles);
			font->print(math::rectf(10, 20 + yoffset, 10, 10), &attr, 0, msg, m_guiRender);
			yoffset += attr.fontSize;
		}

	}

	m_guiRender->Flush();
	getDevice()->useShader(0);
}

void Application::onRenderDone(scene::ViewPort*vp)
{
}
void Application::WindowPostRender(video::RenderWindow* wnd)
{
	getDevice()->set2DMode();
}
void Application::update(float dt)
{
	CMRayApplication::update(dt);



}
void Application::onDone()
{
	//NCAppGlobals::Instance()->Save("VTSettings.conf");
	//WriteSettingsXML();
}

}
