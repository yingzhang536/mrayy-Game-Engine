

#include "stdafx.h"
#include "VTelesarRenderingState.h"

#include "GUIBatchRenderer.h"

#include "FontResourceManager.h"


#include "IThreadManager.h"
#include "IThread.h"
#include "UDPCommunicationLayer.h"

#include "StreamReader.h"
#include "DebugRenderSettings.h"
#include "IVTDebugInterface.h"
#include "PhysXManager.h"
#include "ATAppGlobal.h"
#include "VTBaseState.h"

#include "DebugDrawManager.h"
#include "GUIThemeManager.h"

#include "ShaderResourceManager.h"
#include "MaterialResourceManager.h"

#include "ParsedShaderPP.h"
#include "SceneComponent.h"

#include "RenderTechnique.h"
#include "RenderPass.h"

#include "HeadMount.h"
#include "CameraConfigurationManager.h"
#include "HeadCameraComponent.h"
#include "CameraComponent.h"
#include "NodeHeadController.h"

#include "CommunicationManager.h"
#include "RobotCommunicatorComponent.h"
#include "XMLTree.h"

namespace mray
{

namespace AugTel
{


	class VTelesarStateImpl
	{

	protected:

	public:
		HeadMount* headMount;


	public:
		VTelesarStateImpl()
		{
			headMount = 0;
		}
		~VTelesarStateImpl()
		{
		}

		void Init()
		{
		}



	};

	class UDPMessageListener :public VT::IUDPCommunicationLayerListener
	{
	public:


		enum EUDPCommand
		{
			LoadScene1 = 0x10,
			LoadScene2,
			LoadScene3,
			LoadScene4,
			LoadScene5
		};
		GCPtr<OS::IMutex> dataMutex;
		std::list<EUDPCommand> commandQueue;
		VTelesarRenderingState* m_owner;
		UDPMessageListener()
		{
		}

		void Prepare()
		{
			dataMutex = OS::IThreadManager::getInstance().createMutex();
		}

		EUDPCommand PopCommand()
		{
			int i = 0;
			dataMutex->lock();
			if (commandQueue.size() == 0)
				i = -1;
			else
			{
				i = *commandQueue.begin();
				commandQueue.erase(commandQueue.begin());
			}
			dataMutex->unlock();
			return (EUDPCommand)i;
		}

		void OnCustomMessage(OS::IStream* stream)
		{
			OS::StreamReader r(stream);
			int i = r.binReadInt();
			dataMutex->lock();
			commandQueue.push_back((EUDPCommand)i);
			dataMutex->unlock();
		}
	}g_udpListener;


VTelesarRenderingState::VTelesarRenderingState(const core::string& name)
	:IRenderingState(name)
{
	m_vtState = new VTBaseState();
	m_data = new VTelesarStateImpl();

	GUI::GUIBatchRenderer*r = new GUI::GUIBatchRenderer();
	r->SetDevice(Engine::getInstance().getDevice());
	m_guiRenderer = r;
	m_stateActive = false;
}
VTelesarRenderingState::~VTelesarRenderingState()
{

	delete m_vtState;
	m_gameManager = 0;
	m_sceneManager = 0;
	m_phManager = 0;
}

void VTelesarRenderingState::_CreatePhysicsSystem()
{
	core::string maxIter = gAppData.GetValue("Physics", "MaxIterations");
	core::string maxTimestepDiv = gAppData.GetValue("Physics", "MaxTimeStepDiv");
	core::string simulationStepDiv = gAppData.GetValue("Physics", "SimulationStepDiv");

	float simStep;

	if (simulationStepDiv == "")
		simStep = 1.0f / 60.0f;
	else simStep = 1.0f / core::StringConverter::toFloat(simulationStepDiv);

	physics::PhysicsSystemDesc desc;
	desc.gravity.set(0, -9.8 * 1, 0);
	if (maxIter == "")
		desc.maxIter = 32;
	else desc.maxIter = core::StringConverter::toInt(maxIter);

	if (maxTimestepDiv == "")
		desc.maxTimestep = 1.0f / (16 * 60.0f);
	else desc.maxTimestep = 1.0f / core::StringConverter::toFloat(maxTimestepDiv);
	desc.useFixedTimeStep = true;
	m_phManager = new physics::PhysXManager(&desc);

	//if(VTAppGlobals::IsDebugging)
	m_phManager->ConnectToRemoteDebugger();

}


bool VTelesarRenderingState::OnEvent(Event* e, const math::rectf& rc)
{
	return false;
}


void VTelesarRenderingState::_LoadEnvironment(const core::string& path)
{
	if (m_loaded)
		return;
	m_loaded = true;
	xml::XMLTree tree;

	if (m_gameManager->loadFromFile("tachilab.xml", 0))
	{

	}

	core::string environmentPath = path;//VTAppGlobals::GetValue("VTelesarRenderingState","Environment");

	if (m_gameManager->loadFromFile(environmentPath, 0))
	{

	}

}
void VTelesarRenderingState::_Clear()
{
	m_gameManager->Clear();
	m_phManager->clearNodes();
	m_phManager->clearJoints();
	//m_phManager->clearMaterials();
	m_sceneManager->clearSceneNodes();


}

void VTelesarRenderingState::_LoadModel(const core::string& scene)
{
	xml::XMLTree tree;

	m_loaded = false;
	std::vector<game::GameEntity*> entLst;
	if (m_gameManager->loadFromFile(scene, &entLst))
	{
	}

	if (VT::CommunicationManager::getInstance().GetCommunicators().size() > 0)
		m_robot = dynamic_cast<VT::RobotCommunicatorComponent*>(*VT::CommunicationManager::getInstance().GetCommunicators().begin());

	if (m_robot && !m_stateActive)
		m_robot->SetEnabled(false);

	if (true)
	{
		scene::LightNode* light = m_sceneManager->createLightNode();
		light->setPosition(math::vector3d(0, 3, 2));
		light->setTarget(0);
		light->setType(scene::LT_SpotLight);

		GCPtr<video::ITexture> tex = Engine::getInstance().getDevice()->createEmptyTexture2D(false);
		tex->setMipmapsFilter(false);
		tex->createTexture(math::vector3d(1024, 1024, 1), video::EPixel_Alpha8);//EPixel_Float16_R);
		video::IRenderTargetPtr rt = Engine::getInstance().getDevice()->createRenderTarget("", tex, 0, 0, 0);
		light->setShadowMap(rt);
		light->setCastShadows(rt);

		math::matrix4x4 projection = math::MathUtil::CreateProjectionMatrixPerspectiveFov(45, 1, 1, 500);

		light->setProjection(projection);
	}
	{
		scene::CameraNode* cam[2];
		video::EPixelFormat pf = video::EPixel_Float16_RGBA;//video::EPixel_R8G8B8A8;//

		HeadMount* hm = new HeadMount(m_sceneManager, 1);
		m_data->headMount = hm;
		m_sceneManager->addSceneNode(hm);
		hm->SetOculus(gAppData.oculusDevice);


		for (int i = 0; i < 2; ++i)
		{

			cam[i] = m_sceneManager->createCamera();
			m_viewport[i] = new scene::ViewPort("", cam[i], 0, 0, math::rectf(0, 0, 1, 1), 0);
			m_viewport[i]->SetClearColor(video::SColor(0, 0, 0, 0));

			video::ITexturePtr renderTargetTex = Engine::getInstance().getDevice()->createTexture2D(math::vector2d(1, 1), pf, true);
			renderTargetTex->setBilinearFilter(false);
			renderTargetTex->setTrilinearFilter(false);
			renderTargetTex->setMipmapsFilter(false);

			video::IRenderTargetPtr rt = Engine::getInstance().getDevice()->createRenderTarget(mT(""), renderTargetTex, video::IHardwareBufferPtr::Null, video::IHardwareBufferPtr::Null, false);
			m_viewport[i]->setRenderTarget(rt);
			m_viewport[i]->setOnlyToRenderTarget(true);
			m_viewport[i]->SetAutoUpdateRTSize(true);
			m_viewport[i]->enablePostProcessing(true);
			m_viewport[i]->AddListener(this);

			if (false)
			{
				renderTargetTex = Engine::getInstance().getDevice()->createTexture2D(1, pf, true);
				renderTargetTex->setBilinearFilter(false);
				renderTargetTex->setTrilinearFilter(false);
				renderTargetTex->setMipmapsFilter(false);
				rt->attachRenderTarget(renderTargetTex, 1);
				if (false)
				{
					renderTargetTex = Engine::getInstance().getDevice()->createTexture2D(1, pf, true);
					renderTargetTex->setBilinearFilter(false);
					renderTargetTex->setTrilinearFilter(false);
					renderTargetTex->setMipmapsFilter(false);
					rt->attachRenderTarget(renderTargetTex, 2);
				}

				video::ParsedShaderPP* pp = new video::ParsedShaderPP(Engine::getInstance().getDevice());
				pp->LoadXML(gFileSystem.openFile("AugTel.peff"));
				m_viewport[i]->setPostProcessing(pp);
			}

			hm->addChild(cam[i]);
			cam[i]->setZNear(0.001);
			cam[i]->setZFar(10);
			
			/*game::GameEntity* ent = entLst[0];
			AugTel::HeadCameraComponent* eyes = ent->RetriveComponent<AugTel::HeadCameraComponent>(ent, "Head");
			VT::CameraComponent* cameraComponent = ent->RetriveComponent<VT::CameraComponent>(ent, "stereoCamera");

			if (eyes)
			{
				eyes->GetNode()->addChild(hm);
				hm->setPosition(eyes->GetOffset());
			}
			else if (cameraComponent)
			{
				cameraComponent->MountCamera(hm, 0);
			}

			if (AppData::Instance()->headController == TBee::EHeadControllerType::SceneNode)
			{
				TBee::NodeHeadController* c = dynamic_cast<TBee::NodeHeadController*>(m_robotConnector->GetHeadController());
				c->SetNode(hm);
				c->SetInitialOrientation(hm->getAbsoluteOrintation());
			}*/

			if (true)
			{
				scene::LightNode* m_lightSrc = m_sceneManager->createLightNode();
				m_lightSrc->setPosition(math::vector3d(0, 50, 0));
				m_lightSrc->setCastShadows(true);

				video::ITexturePtr t = gEngine.getDevice()->createEmptyTexture2D(false);
				t->setMipmapsFilter(false);
				t->createTexture(math::vector3di(2048, 2048, 1), video::EPixel_Float16_R);
				m_lightSrc->setShadowMap(gEngine.getDevice()->createRenderTarget("", t, 0, 0, 0));
			}

		}

		m_data->headMount->SetCamera(m_camera[0], m_camera[1]);
		m_attachToHead = false;
		_SwitchCamera(m_data->headMount, SwitchHead);

		game::GameEntity* ent = entLst[0];
		AugTel::HeadCameraComponent* eyes = ent->RetriveComponent<AugTel::HeadCameraComponent>(ent, "Head");
		VT::CameraComponent* cameraComponent = ent->RetriveComponent<VT::CameraComponent>(ent, "stereoCamera");

		if (eyes)
		{
			eyes->GetNode()->addChild(m_data->headMount);
			m_data->headMount->setPosition(eyes->GetOffset());
		}
		else if (cameraComponent)
		{
			cameraComponent->MountCamera(m_data->headMount, 0);
		}

	}
}


void VTelesarRenderingState::_SwitchCamera(scene::IMovable* m, ECameraMode mode)
{
	switch (mode)
	{
	case SwitchHead:
	{
		scene::StereoRenderer* sr = 0;
		if (dynamic_cast<HeadMount*>(m))
		{
			m_attachToHead = !m_attachToHead;
			((HeadMount*)m)->SetDisabled(m_attachToHead);
		}
		if (m->getParent() == 0)
		{
			if (m_robot && m_robot->GetCameraComponent())
			{
				m_robot->GetCameraComponent()->MountCamera(m, sr);
			}
			//					m_head->addChild(m_headMount);
		}
		else
		{
			m->setPosition(m->getAbsolutePosition());
			m->setOrintation(m->getAbsoluteOrintation());
			m->removeFromParent();
		}
	}break;
	case Front:
	{
		if (dynamic_cast<HeadMount*>(m))
		{
			m_attachToHead = false;
			((HeadMount*)m)->SetDisabled(m_attachToHead);
		}
		if (m->getParent() != 0)
		{
			m->setPosition(m->getAbsolutePosition());
			m->setOrintation(math::quaternion(0, m->getAbsoluteOrintation().getYaw(), 0));
			m->removeFromParent();
		}
		if (m_robot && m_robot->GetCameraComponent())
		{
			m->setPosition(m_robot->GetCameraComponent()->GetAbsolutePosition() + m_robot->GetCameraComponent()->GetAbsoluteOrientation()*math::vector3d(0, 0.3, 1));
			math::quaternion q(math::MathUtil::CreateLookAtMatrix(m_robot->GetCameraComponent()->GetAbsolutePosition(), m->getPosition(), math::vector3d::YAxis));
			m->setOrintation(q);
		}
		//					m_head->addChild(m_headMount);
	}break;
	}
}

void VTelesarRenderingState::InitState()
{
	IRenderingState::InitState();

	{
		_CreatePhysicsSystem();
		m_gameManager = new game::GameEntityManager();
		m_sceneManager = new scene::SceneManager(Engine::getInstance().getDevice());
		m_guiManager = new GUI::GUIManager(Engine::getInstance().getDevice());
		m_debugRenderer = new scene::DebugDrawManager(Engine::getInstance().getDevice());

		m_guiManager->SetActiveTheme(GUI::GUIThemeManager::getInstance().getActiveTheme());
		m_guiroot = (GUI::IGUIPanelElement*)new GUI::IGUIPanelElement(core::string(""), m_guiManager);
		m_guiroot->SetDocking(GUI::EED_Fill);
		m_guiManager->SetRootElement(m_guiroot);

		m_gameManager->SetPhysicsManager(m_phManager);
		m_gameManager->SetSceneManager(m_sceneManager);


		gAppData.guiManager = m_guiManager;
	}
	{
		gShaderResourceManager.parseShaderXML(gFileSystem.openFile("AT_Shaders.shd"));
		gMaterialResourceManager.parseMaterialXML(gFileSystem.openFile("VT_materials.mtrl"));
	}
	{
		m_data->Init();
	}
	{
		_LoadEnvironment(m_environment);
		_LoadModel(m_model);
	}
}

void VTelesarRenderingState::OnEnter(IRenderingState*prev)
{
	IRenderingState::OnEnter(prev);

	gAppData.headObject = m_data->headMount;

	m_stateActive = true;

	if (m_robot )
		m_robot->SetEnabled(true);
}

void VTelesarRenderingState::OnExit()
{
	IRenderingState::OnExit();

	gAppData.cameraProvider = 0;
	m_stateActive = false;

	if (m_robot)
		m_robot->SetEnabled(false);
}
void VTelesarRenderingState::onRenderBegin(scene::ViewPort*vp)
{
	ETargetEye eye;
	if (vp == m_viewport[GetEyeIndex(Eye_Left)])
	{
		eye = Eye_Left;
	}
	else
	{
		eye = Eye_Right;
	}
	/*
	Engine::getInstance().getDevice()->set2DMode();
	video::TextureUnit tu;
	tu.SetTexture(gTextureResourceManager.loadTexture2D("00353_full.jpg"));
	Engine::getInstance().getDevice()->useTexture(0, &tu);
	Engine::getInstance().getDevice()->draw2DImage(vp->getAbsRenderingViewPort(), 1);
	*/

	Engine::getInstance().getDevice()->set3DMode();

}
void VTelesarRenderingState::onRenderDone(scene::ViewPort*vp)
{

	m_debugRenderer->EndDraw();
}


void VTelesarRenderingState::_RenderUI(const math::rectf& rc, math::vector2d& pos)
{
// 	if (!ATAppGlobal::Instance()->IsDebugging)
// 		return;
	GUI::IFont* font = gFontResourceManager.getDefaultFont();
	GUI::FontAttributes attr;
	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	AppData* app = AppData::Instance();


	video::TextureUnit tu;
	dev->useTexture(0, &tu);
	dev->draw2DImage(math::rectf(0, 0, 200, 200), 1);
	if (font)
	{
		attr.fontColor.Set(1, 1, 1, 1);
		attr.fontAligment = GUI::EFA_MiddleLeft;
		attr.fontSize = 18;
		attr.hasShadow = true;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;

		math::rectf r = rc;
		r.ULPoint = pos;

		font->print(r, &attr, 0, core::string("Test"), m_guiRenderer);

		m_guiRenderer->Flush();
	}
}

#define DRAW_GRID false

video::IRenderTarget* VTelesarRenderingState::Render(const math::rectf& rc, ETargetEye eye)
{

	video::IVideoDevice* device = Engine::getInstance().getDevice();
	int index = GetEyeIndex(eye);

	if (ATAppGlobal::Instance()->IsDebugging)
	{
		m_gameManager->DebugRender(m_debugRenderer);
		if (DRAW_GRID)
		{
			int n = 20;
			for (int i = 0; i < n; ++i)
			{
				m_debugRenderer->AddLine(math::vector3d(-10, 0, 0.5f*(i - n / 2)), math::vector3d(10, 0, 0.5f*(i - n / 2)), video::DefaultColors::Gray);
				m_debugRenderer->AddLine(math::vector3d(0.5f*(i - n / 2), 0, -10), math::vector3d(0.5f*(i - n / 2), 0, 10), video::DefaultColors::Gray);
			}
		}
	}

	{
		m_debugRenderer->StartDraw(m_viewport[index]->getCamera());
		m_viewport[index]->setAbsViewPort(rc);
		m_viewport[index]->draw();
	}


	math::rectf vprect = rc;
	video::TextureUnit tex;
	//Parent::Render(vp->getAbsRenderingViewPort(), eye);
	IRenderingState::Render(rc, eye);

	device->setRenderTarget(m_renderTarget[index]);
	if (AppData::Instance()->IsDebugging && false)
		m_gameManager->GUIRender(m_guiRenderer, vprect);

	{
		tex.SetTexture(m_viewport[index]->getRenderTarget()->GetColorTexture());
		device->useTexture(0, &tex);
		math::rectf tc = math::rectf(0, 0, 1, 1);
		device->draw2DImage(vprect, 1, 0, &tc);
	}
	math::rectf vp(0, m_renderTarget[index]->GetSize());
	m_guiManager->DrawAll(&vp);
	math::vector2d pos = rc.ULPoint;
	_RenderUI(rc, pos);
	return m_renderTarget[index];


}

void VTelesarRenderingState::Update(float dt)
{
	IRenderingState::Update(dt);
	m_sceneManager->update(dt);
	m_gameManager->Update(dt);
	m_guiManager->Update(dt);
	m_phManager->update(dt);
	m_debugRenderer->Update(dt);

	controllers::IKeyboardController* kb = gAppData.inputMngr->getKeyboard();

	m_vtState->Update(dt);

}
void VTelesarRenderingState::LoadFromXML(xml::XMLElement* e)
{
	IRenderingState::LoadFromXML(e);
	m_model = e->getValueString("Model");
	m_environment = e->getValueString("Environment");
}

}
}

