

#include "stdafx.h"
#include "AugCameraRenderState.h"

#include "TextureResourceManager.h"
#include "MeshFileCreatorManager.h"
#include "ShaderResourceManager.h"
#include "ATAppGlobal.h"
#include "OpenNIManager.h"
#include "OpenNIUtils.h"
#include "GUIThemeManager.h"
#include "IGUIPanelElement.h"

#include "PhysXManager.h"

#include "OptiTrackClient.h"

#include "VTLib.h"
#include "RobotCommunicator.h"
#include "UDPCommunicationLayer.h"
#include "VTSharedMemory.h"
#include "CommunicationManager.h"
#include "OptiTrackDataSource.h"

#include "HeadCameraComponent.h"
#include "CameraComponent.h"
#include "HeadMount.h"
#include "GameComponentCreator.h"
#include "DebugDrawManager.h"
#include "ICameraVideoSource.h"

#include "CRobotConnector.h"
#include "TBRobotInfo.h"
#include "RobotInfoManager.h"

#include "OpenNIHandler.h"

#include "MeshBufferData.h"
#include "IMeshBuffer.h"
#include "IHardwareStreamBuffer.h"
#include "RenderMaterial.h"
#include "RenderTechnique.h"
#include "RenderPass.h"
#include "DepthVisualizer.h"
#include "DataCommunicator.h"
#include "ParsedShaderPP.h"

#include "ATAppGlobal.h"
#include "RenderWindow.h"
#include "Application.h"

#include "LocalRobotCommunicator.h"

#define VT_USING_SHAREDMEM
#define VIDEO_PORT 5000
#define AUDIO_PORT 5002
#define COMMUNICATION_PORT 5003

#define DEPTH_LOCAL
namespace mray
{
namespace AugTel
{

	class AugCameraStateImpl
	{
	public:
		HeadMount* headMount;

	public:
		AugCameraStateImpl()
		{
			headMount = 0;
		}
		~AugCameraStateImpl()
		{
		}


	};

	class TelesarCommunicationHandler
	{
		static int _refCount;
		static TelesarCommunicationHandler* _instance;
	public:

		VT::ICommunicationLayer* commLayer;

	protected:

		TelesarCommunicationHandler()
		{
			commLayer = 0;
		}
		~TelesarCommunicationHandler()
		{
			delete commLayer;
		}
		void _init()
		{


#ifdef VT_USING_SHAREDMEM
			commLayer = new VT::VTSharedMemory(mT("TelesarV"));
#else
			commLayer = new VT::UDPCommunicationLayer(1234);
#endif
			VT::CommunicationManager::getInstance().AddCommunicationLayer(mT("TelesarV"), commLayer);


			commLayer->Start();

			game::GameComponentCreator::getInstance().AddAlias("KinematicJointDOF6Component", "PhysicalJointComponent");
			game::GameComponentCreator::getInstance().AddAlias("NullPhysicsComponent", "RigidBodyComponent");
		}

	public:
		static void Ref()
		{
			_refCount++;
			if (_refCount == 1)
			{
				_instance = new TelesarCommunicationHandler();
				_instance->_init();
			}
		}
		static void Unref()
		{
			_refCount--;
			if (_refCount == 0)
			{
				delete _instance;
				_instance = 0;
			}
		}
		static TelesarCommunicationHandler* Instance(){ return _instance; }

		void Update(float dt)
		{
			commLayer->Update(dt);
		}
	};
	int TelesarCommunicationHandler::_refCount=0;
	TelesarCommunicationHandler* TelesarCommunicationHandler::_instance=0;

AugCameraRenderState::AugCameraRenderState(TBee::ICameraVideoSource* src)
{
	m_data = new AugCameraStateImpl();

	m_eyes[0].flip90 = 0;
	m_eyes[1].flip90 = 0;

	m_eyes[0].cw = 0;
	m_eyes[1].cw = 1;

	m_camVideoSrc = src;

	SetVideoSource(m_camVideoSrc);

	m_openNiHandler = new TBee::OpenNIHandler();
	m_openNiHandler->SetScale(0.5);
	m_depthTime = 0;

	m_depthVisualizer = new DepthVisualizer();
	m_robotConnector->SetCommunicator(new TBee::LocalRobotCommunicator());

	m_viewDepth = false;

}
AugCameraRenderState::~AugCameraRenderState()
{
	delete m_data;
	delete m_camVideoSrc;
	m_gameManager = 0;
	m_sceneManager = 0;
	m_phManager = 0;
	TelesarCommunicationHandler::Unref();
	delete m_openNiHandler;
	delete m_depthVisualizer;
}


bool AugCameraRenderState::OnEvent(Event* e, const math::rectf& rc)
{
	if (Parent::OnEvent(e, rc))
		return true;
	bool ok = false;
	if (m_guiManager)
	{
		if (m_guiManager->OnEvent(e, &rc))
			return true;
	}
	if (e->getType() == ET_Keyboard)
	{
		KeyboardEvent* evt = (KeyboardEvent*)e;
		if (evt->press)
		{
			if ( evt->key == KEY_F11)
			{
				gTextureResourceManager.writeResourceToDist(m_renderTarget[0]->getColorTexture(), "depth.tga");
			}
			else
			if ( evt->key == KEY_R)
			{
				m_data->headMount->SetDisabled(!m_data->headMount->IsDisabled());
				ok = true;
			}
			else if ( evt->key == KEY_F10)
			{
				core::string name = "screenShots/ScreenShot_";
				name += core::StringConverter::toString(gTimer.getActualTime());
				name += ".png";
				gTextureResourceManager.writeResourceToDist(m_rtTexture[0], name);

				ok = true;
			}
			else if (evt->key == KEY_D)//show/hide depth
			{
				m_viewDepth = !m_viewDepth;
				ok = true;
			}
			else
			if (evt->key == KEY_A)//Show/Hide arms
			{
				m_showScene = !m_showScene;
				ok = true;
			}
			else if (evt->key == KEY_SPACE )
			{
				if (m_robotConnector->IsRobotConnected())
					m_robotConnector->EndUpdate();
				else
					m_robotConnector->StartUpdate();
				ok = true;
			}
			else if (evt->key == KEY_ESCAPE)// && evt->lshift)
			{
				m_exitCode = STATE_EXIT_CODE;
				ok = true;
			}
			else
			if (evt->key == KEY_C)
			{
				TelesarCommunicationHandler::Instance()->commLayer->InjectCommand("calib", "");
				ok = true;
			}
		}
	}
	return ok;
}



void AugCameraRenderState::_CreatePhysicsSystem()
{
	core::string maxIter = ATAppGlobal::Instance()->GetValue("Physics", "MaxIterations");
	core::string maxTimestepDiv = ATAppGlobal::Instance()->GetValue("Physics", "MaxTimeStepDiv");
	core::string simulationStepDiv = ATAppGlobal::Instance()->GetValue("Physics", "SimulationStepDiv");

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

void AugCameraRenderState::InitState()
{
	Parent::InitState();

	//init Virtual Telesar Library
	if (true)
	{
		TelesarCommunicationHandler::Ref();
	}
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

		m_showScene = true;

		ATAppGlobal::Instance()->guiManager = m_guiManager;
	}
	{
		m_camVideoSrc->Init();
	}

	{
		gShaderResourceManager.parseShaderXML(gFileSystem.openFile("AT_Shaders.shd"));
		gMaterialResourceManager.parseMaterialXML(gFileSystem.openFile("AT_materials.mtrl"));
	}

	{
		std::vector<game::GameEntity*> entLst;
		m_gameManager->loadFromFile(m_model, &entLst);

		HeadMount* hm = new HeadMount(m_sceneManager, 1);
		scene::CameraNode* cam[2];
		video::EPixelFormat pf = video::EPixel_Float16_RGBA;//video::EPixel_R8G8B8A8;//


		for (int i = 0; i < 2; ++i)
		{

			cam[i] = m_sceneManager->createCamera();
			m_viewport[i] = new scene::ViewPort("", cam[i], 0, 0, math::rectf(0, 0, 1, 1), 0);
			m_viewport[i]->SetClearColor(video::SColor(0,0,0,0));

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

			//if (ATAppGlobal::Instance()->oculusDevice)
			cam[i]->setFovY(m_hmdFov);
			cam[i]->setPosition(math::vector3d(0.03 - 0.06*i,0.07,0));
			m_camera[i] = cam[i];
		}

		game::GameEntity* ent = entLst[0];
		hm->SetOculus(ATAppGlobal::Instance()->oculusDevice);
		m_sceneManager->addSceneNode(hm);
		m_data->headMount = hm;
		m_data->headMount->SetCamera(m_camera[0], m_camera[1]);
		AugTel::HeadCameraComponent* eyes = ent->RetriveComponent<AugTel::HeadCameraComponent>(ent, "Head");
		VT::CameraComponent* cameraComponent = ent->RetriveComponent<VT::CameraComponent>(ent,"stereoCamera");
		if (eyes)
		{
			eyes->GetNode()->addChild(hm);
			hm->setPosition(eyes->GetOffset());
		}
		else if (cameraComponent)
		{
			cameraComponent->MountCamera(hm, 0);
		}

		scene::LightNode* lightSrc= m_sceneManager->createLightNode();
		lightSrc->setPosition(math::vector3d(50, 50, 50));
	}

	{
#ifdef DEPTH_LOCAL
		m_openNiHandler->Init();
#else
		m_openNiHandler->CreateDepthFrame(320, 240);
#endif
		m_openNiHandler->GetNormalCalculator().SetCalculateNormals(true);
		math::vector2di sz = m_openNiHandler->GetSize();

		m_depthVisualizer->Init();
		m_depthVisualizer->SetViewNormals(true);
		m_depthVisualizer->SetDepthFrame(m_openNiHandler->GetNormalCalculator().GetDepthFrame());

		video::RenderMaterial* mtrl = new video::RenderMaterial();
		video::RenderPass* pass= mtrl->CreateTechnique("D")->CreatePass("D");
		pass->setRenderState(video::RS_Wireframe, video::ES_Use);
		pass->setRenderState(video::RS_Lighting, video::ES_DontUse);

		m_depthMesh = new scene::SMesh();
		scene::MeshBufferData* buffData= m_depthMesh->addNewBuffer();
		scene::MeshGenerator::getInstance().generatePlane(199,199, buffData->getMeshBuffer());

		buffData->setMaterial(mtrl);

		mtrl = gMaterialResourceManager.getMaterial("GreenMtrl");
		scene::ISceneNode* node = m_sceneManager->createSceneNode();
		scene::MeshRenderableNode* rnode = new scene::MeshRenderableNode(m_depthMesh);
		rnode->setMaterial(mtrl, 0);
		node->AttachNode(rnode);
		node->setScale(math::vector3d(1.5, 1, 1.5));
		node->setOrintation(math::quaternion(-90, math::vector3d::XAxis)*math::quaternion(180, math::vector3d::YAxis));
		m_camera[0]->addChild(node);
		node->setPosition(math::vector3d(0, 0, 1));

		node->setVisible(false);
		m_depthNode = node;
			/*
		buffData->getMeshBuffer()->createStream(0, video::EMeshStreamType::EMST_Position, video::EStreamDataType::ESDT_Point3f, sz.x*sz.y, video::IHardwareBuffer::EUT_Dynamic, true, true);
		buffData->getMeshBuffer()->createStream(0, video::EMeshStreamType::EMST_Color, video::EStreamDataType::ESDT_Point4f, sz.x*sz.y, video::IHardwareBuffer::EUT_Dynamic, true, true);
		buffData->getMeshBuffer()->createIndexBuffer(video::IHardwareIndexBuffer::EIT_16Bit,(sz.x-1)*(sz.y-1))*/
	}

}

void AugCameraRenderState::OnEnter(IRenderingState*prev)
{
	Parent::OnEnter(prev);
	ATAppGlobal::Instance()->optiDataSource->Connect(m_optiProvider);
#ifdef DEPTH_LOCAL
	m_openNiHandler->Start(320, 240);
#endif
	ATAppGlobal::Instance()->headObject = m_data->headMount;
	ATAppGlobal::Instance()->depthProvider = m_openNiHandler;

	m_camVideoSrc->Open();

	ATAppGlobal::Instance()->dataCommunicator->Start(COMMUNICATION_PORT);

	TBee::TBRobotInfo* ifo = AppData::Instance()->robotInfoManager->GetRobotInfo(0);
	if(ifo)
		m_robotConnector->ConnectRobotIP(ifo->IP, VIDEO_PORT, AUDIO_PORT, COMMUNICATION_PORT);
	m_robotConnector->SetData("depthSize", "", false);

}

void AugCameraRenderState::OnExit()
{
	Parent::OnExit();
	ATAppGlobal::Instance()->optiDataSource->Disconnect();
	m_camVideoSrc->Close();

	m_openNiHandler->Close();

	ATAppGlobal::Instance()->dataCommunicator->Stop();
}
void AugCameraRenderState::onRenderBegin(scene::ViewPort*vp)
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
void AugCameraRenderState::onRenderDone(scene::ViewPort*vp)
{

	m_debugRenderer->EndDraw();
}

void AugCameraRenderState::_CalculateDepthGeom()
{
	if (!m_depthNode->isVisible())
		return;
	scene::IMeshBuffer* buff= m_depthMesh->getBuffer(0);
	video::IHardwareStreamBuffer* pos = buff->getStream(0, video::EMST_Position);
	video::IHardwareStreamBuffer* norm = buff->getStream(0, video::EMST_Normal);
	math::vector3d* posPtr = (math::vector3d*) pos->lock(0, 0, video::IHardwareStreamBuffer::ELO_Normal);
	math::vector3d* nPtr = (math::vector3d*) norm->lock(0, 0, video::IHardwareStreamBuffer::ELO_Discard);

	math::vector2di sz = m_openNiHandler->GetSize();
	float* depthPtr = m_openNiHandler->GetNormalCalculator().GetDepthData();
	math::vector3d* normPtr = m_openNiHandler->GetNormalCalculator().GetNormalData();
	if (depthPtr && normPtr)
	{
		for (int i = 0; i < 200; ++i)
		{
			int i2 = (i*sz.x / 200);
			for (int j = 0; j < 200; ++j)
			{
				int j2 = (j*sz.y / 200);

				int idx = j*200 + i;
				int idx2 = j2*sz.x + i2;
				float v = depthPtr[idx2];

				if (v>1)
					v = 1;
				else if (v < 0.1)
					v = 1;
				v = 1 - v;
				//v = sin((float)i / (float)sz.x);
				nPtr[idx] = normPtr[idx2];
				posPtr[idx].y = v;
			}
		}
	}

	pos->unlock();
	norm->unlock();
}
#define DRAW_GRID false

video::IRenderTarget* AugCameraRenderState::Render(const math::rectf& rc, ETargetEye eye)
{
	if (m_depthTime > 0.3)
	{
		m_depthTime = 0;
		m_robotConnector->SetData("depth#1", core::StringConverter::toString(math::recti(0, 0, 320, 240)), false);
	}

	_CalculateDepthGeom();

	m_depthVisualizer->Update();

	video::IVideoDevice* device = Engine::getInstance().getDevice();
	int index = GetEyeIndex(eye);
	m_camVideoSrc->Blit();

	if (AppData::Instance()->IsDebugging )
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

	if (m_showScene)
	{
		m_debugRenderer->StartDraw(m_viewport[index]->getCamera());
		m_viewport[index]->setAbsViewPort(rc);
		m_viewport[index]->draw();
	}


	math::rectf vprect = rc;
	video::TextureUnit tex;
	//Parent::Render(vp->getAbsRenderingViewPort(), eye);
	Parent::Render(rc, eye);

	if (/*m_openNiHandler->IsStarted() &&*/ m_viewDepth)
	{
		tex.SetTexture(m_depthVisualizer->GetNormalsTexture());
		device->useTexture(0, &tex);
		math::rectf r(m_openNiHandler->GetCenter() - m_openNiHandler->GetScale()*0.5, m_openNiHandler->GetCenter() + m_openNiHandler->GetScale()*0.5);
		r.ULPoint *= rc.getSize();
		r.BRPoint *= rc.getSize();

		device->draw2DImage(r,video::SColor(1,1,1,0.5));
	//	m_openNiHandler->Render(rc, 0.5f);
	}
	if (AppData::Instance()->IsDebugging && false)
		m_gameManager->GUIRender(Parent::m_guiRenderer, vprect);

	if (m_showScene)
	{
		tex.SetTexture(m_viewport[index]->getRenderOutput()->getColorTexture());
		device->useTexture(0, &tex);
		math::rectf tc = math::rectf(0, 0, 1, 1);
		device->draw2DImage(vprect, 1, 0, &tc);
	}
	math::rectf vp(0, m_renderTarget[index]->getSize());
	m_guiManager->DrawAll(&vp);
	return m_renderTarget[index];


}

void AugCameraRenderState::Update(float dt)
{
	Parent::Update(dt);
	m_sceneManager->update(dt);
	m_gameManager->Update(dt);
	m_guiManager->Update(dt);
	m_phManager->update(dt);
	m_debugRenderer->Update(dt);

	m_openNiHandler->Update(dt);

	controllers::IKeyboardController* kb= ATAppGlobal::Instance()->inputMngr->getKeyboard();

	math::vector2d s = m_openNiHandler->GetScale();
	s.x += (kb->getKeyState(KEY_K) - kb->getKeyState(KEY_J))*dt*0.5f;
	s.y += (kb->getKeyState(KEY_I) - kb->getKeyState(KEY_M))*dt*0.5f;

	math::vector2d c = m_openNiHandler->GetCenter();
	c.x += (kb->getKeyState(KEY_RIGHT) - kb->getKeyState(KEY_LEFT))*dt*0.5f;
	c.y -= (kb->getKeyState(KEY_UP) - kb->getKeyState(KEY_DOWN))*dt*0.5f;
	m_openNiHandler->SetScale(s);
	m_openNiHandler->SetCenter(c);

	TelesarCommunicationHandler::Instance()->Update(dt);

	m_depthTime += dt;
}
void AugCameraRenderState::LoadFromXML(xml::XMLElement* e)
{
	IEyesRenderingBaseState::LoadFromXML(e);
	m_camVideoSrc->LoadFromXML(e);

	m_model = e->getValueString("Model");
	m_optiProvider = e->getValueString("OptiProvider");
}

}
}

