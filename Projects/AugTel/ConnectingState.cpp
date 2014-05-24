

#include "stdafx.h"
#include "ConnectingState.h"

#include "ATAppGlobal.h"
#include "PhysXManager.h"

#include "GUIThemeManager.h"
#include "IGUIPanelElement.h"

#include "ParsedShaderPP.h"

#include "HeadMount.h"
#include "FontResourceManager.h"

#include "VisItemDescComponent.h"
#include "ShaderResourceManager.h"
#include "SFModSoundmanager.h"

#include "GUIConnectingScreenImpl.h"
#include "GUIOverlayManager.h"
#include "GUIOverlay.h"

namespace mray
{
namespace AugTel
{


	class ObjectController
	{
	public:
		enum EMode
		{
			EPointFollow,
			EZoomOut
		};
	protected:
		math::vector3d m_target;
		float m_minOffset;
		float m_maxOffset;
		
		enum EState
		{
			Step0,
			Step1,
			Step2,
			Step3,
			Step4,
			Done
		}m_state;
		EMode m_mode;
		float m_speed ;

		float m_timeOut;
		scene::IMovable* m_node;
	public:
		ObjectController()
		{
			m_node = 0;
			m_minOffset = 0.8;
			m_maxOffset = 1.5;
			m_state = Done;
			m_timeOut = 0;
			m_speed = 1.5;
			m_mode = EPointFollow;
		}

		void SetMode(EMode m)
		{
			m_mode = m;
		}
		void SetNode(scene::IMovable* n){ m_node = n; }
		void SetTarget(const math::vector3d& pos)
		{
			m_target = pos;

			m_state = Step0;
		}

		void Update(float dt)
		{
			if (!m_node)
				return;
			math::vector3d pos = m_node->getPosition();
			if (m_mode == EPointFollow)
			{
				switch (m_state)
				{
				case Step0:
					if (abs(pos.z - m_target.z) < m_maxOffset)
					{
						pos.z -= m_speed * dt;
						if (abs(pos.z - m_target.z) > m_maxOffset)
							pos.z = m_target.z - m_maxOffset;
					}
					else
						m_state = Step1;
					break;
				case Step1:
					if (abs(pos.y - m_target.y) > 0)
					{
						float delta = math::sign(pos.y - m_target.y)*m_speed * dt;
						if (abs(pos.y - m_target.y)<abs(delta))
							delta = pos.y - m_target.y;
						pos.y -= delta;
					}
					else
						m_state = Step2;
					break;
				case Step2:
					if (abs(pos.z - m_target.z) > m_minOffset)
					{
						pos.z += m_speed * dt;
						if (abs(pos.z - m_target.z) < m_minOffset)
							pos.z = m_target.z - m_minOffset;
					}
					else
						m_state = Step3;
					break;
				case Step3:
					m_timeOut = 3;
					m_state = Step4;
					break;
				case Step4:
					m_timeOut -= dt;
					if (m_timeOut <= 0)
						m_state = Done;
					break;
				}
			}
			else if (m_mode == EZoomOut)
			{
				if (abs(pos.y - m_target.y*2) > 0)
				{
					float delta = math::sign(pos.y - m_target.y)*m_speed * dt;
					if (abs(pos.y - m_target.y) < abs(delta))
						delta = pos.y - m_target.y;
					pos.y -= delta;
				}
				if (abs(pos.z - m_target.z * 2) < m_maxOffset)
				{
					pos.z -= m_speed * dt;
					if (abs(pos.z - m_target.z * 2) > m_maxOffset)
						pos.z = m_target.z * 2 - m_maxOffset;
				}
			}
			m_node->setPosition(pos);
		}
	
		bool IsDone()
		{
			return m_state == Done;
		}
	};
	

	ConnectingState::ConnectingState(const core::string& name)
		:IRenderingState(name)
{
	m_connectScreen = new GUI::GUIConnectingScreenImpl;
}


ConnectingState::~ConnectingState()
{
	delete m_connectScreen;
}


void ConnectingState::_CreatePhysicsSystem()
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

void ConnectingState::InitState()
{
	Parent::InitState();
	{
		_CreatePhysicsSystem();
		m_gameManager = new game::GameEntityManager();
		m_sceneManager = new scene::SceneManager(Engine::getInstance().getDevice());
		m_guiManager = new GUI::GUIManager(Engine::getInstance().getDevice());
		m_soundManager = new sound::SFModSoundManager();

		m_guiManager->SetActiveTheme(GUI::GUIThemeManager::getInstance().getActiveTheme());
		m_guiroot = (GUI::IGUIPanelElement*)new GUI::IGUIPanelElement(core::string(""), m_guiManager);
		m_guiroot->SetDocking(GUI::EED_Fill);
		m_guiManager->SetRootElement(m_guiroot);

		m_gameManager->AddListener(this);
		m_gameManager->SetPhysicsManager(m_phManager);
		m_gameManager->SetSceneManager(m_sceneManager);

		ATAppGlobal::Instance()->guiManager = m_guiManager;
	}
	{
		m_bgmSnd=m_soundManager->loadSound("sounds/Vent_full.mp3", true, sound::ESNDT_2D);
		m_bgmSnd->setLooping(true);
	}
	{
		gShaderResourceManager.parseShaderXML(gFileSystem.openFile("AT_Shaders.shd"));
		gMaterialResourceManager.parseMaterialXML(gFileSystem.openFile("AT_materials.mtrl"));
	}

	{

		GUI::GUIOverlay* screenOverlay = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIConnectingScreen.gui");
		screenOverlay->CreateElements(m_guiManager, m_guiroot, 0, m_connectScreen);
	}
	m_model = "TelesarDesc.xml";

	{

		m_modelParent = m_sceneManager->createSceneNode();

		std::vector<game::GameEntity*> entLst;
		m_gameManager->loadFromFile(m_model, &entLst);

		HeadMount* hm = new HeadMount(m_sceneManager, 1);
		hm->SetDisabled(false);
		hm->setPosition(math::vector3d(-0));
		scene::CameraNode* cam[2];
		video::EPixelFormat pf = video::EPixel_Float16_RGBA;// EPixel_Float16_RGBA;//video::EPixel_R8G8B8A8;//


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

			if (true)
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
			cam[i]->setZNear(0.01);
			cam[i]->setZFar(100);

			//if (ATAppGlobal::Instance()->oculusDevice)
			cam[i]->setFovY(45);
			cam[i]->setPosition(math::vector3d(0.03 - 0.06*i, 0.07, 0));
			m_camera[i] = cam[i];
		}

		game::GameEntity* ent = entLst[0];
		hm->SetOculus(ATAppGlobal::Instance()->oculusDevice);
		m_sceneManager->addSceneNode(hm);
		/*
		m_data->headMount = hm;
		m_data->headMount->SetCamera(m_camera[0], m_camera[1]);
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
		}*/

		scene::LightNode* lightSrc = m_sceneManager->createLightNode();
		lightSrc->setPosition(math::vector3d(50, 50, 50));
		m_cameraController = new ObjectController;
		m_cameraController->SetNode(hm);

	}

}


bool ConnectingState::OnEvent(Event* e, const math::rectf& rc)
{
	return false;
}


void ConnectingState::OnEnter(IRenderingState*prev)
{
	m_bgmSnd->play();
	m_bgmSnd->setVolume(0);
	m_currItem = 0;
	m_connectScreen->ClearStatus();
	m_cameraController->SetTarget(m_descItems[m_currItem]->GetSceneNode()->getAbsolutePosition());
	m_connectScreen->SetStatus(m_descItems[m_currItem]->GetDescription(), true);
	m_cameraController->SetMode(ObjectController::EPointFollow);
	m_state = EConnecting;

}


void ConnectingState::OnExit()
{
	Parent::OnExit();
	m_bgmSnd->stop();
}


video::IRenderTarget* ConnectingState::Render(const math::rectf& rc, TBee::ETargetEye eye)
{
	int index = GetEyeIndex(eye);

	m_viewport[index]->setAbsViewPort(rc);
	m_viewport[index]->draw();

	video::TextureUnit tex;
	math::rectf vprect = rc;
	video::IVideoDevice* device = Engine::getInstance().getDevice();

	Parent::Render(rc, eye);
	device->setRenderTarget(m_renderTarget[GetEyeIndex(eye)]);

	tex.SetTexture(m_viewport[index]->getRenderOutput()->getColorTexture());
	device->useTexture(0, &tex);
	math::rectf tc = math::rectf(0, 0, 1, 1);
	device->draw2DImage(vprect, 1, 0, &tc);

	//	dev->set2DMode();
	//Engine::getInstance().getDevice()->draw2DRectangle(rc, video::DefaultColors::Black);
	math::rectf vp(0, m_renderTarget[GetEyeIndex(eye)]->getSize());
	m_guiManager->DrawAll(&vp);

	GUI::GUIBatchRenderer renderer;
	renderer.SetDevice(Engine::getInstance().getDevice());
	GCPtr<GUI::IFont> font = gFontResourceManager.getDefaultFont();
	if (font){
		renderer.Prepare();

		float yoffset = 50;


		GUI::FontAttributes attr;
		attr.fontColor.Set(1, 1, 1, 1);
		attr.fontAligment = GUI::EFA_MiddleLeft;
		attr.fontSize = 14;
		attr.hasShadow = true;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;
		core::string msg;
		if (m_state == EConnecting)
		{
			msg = mT("Current Component= ");
			msg += m_descItems[m_currItem]->GetDescription();// core::StringConverter::toString(device->getBatchDrawnCount());
		}
		else if (m_state == EConnected)
		{
			m_cameraController->SetMode(ObjectController::EZoomOut);
			m_cameraController->SetTarget(math::vector3d(0, 0.5, 0));

			msg = mT("System Initialized. ");
		}
		font->print(math::rectf(rc.BRPoint.x - 300, rc.BRPoint.y - 100 + yoffset, 10, 10), &attr, 0, msg, &renderer);
		yoffset += attr.fontSize;

	}
	renderer.Flush();

	return m_renderTarget[GetEyeIndex(eye)];
}


void ConnectingState::Update(float dt)
{
	m_sceneManager->update(dt);
	m_phManager->update(dt);
	m_guiManager->Update(dt);
	m_gameManager->Update(dt);
	m_soundManager->runSounds(dt);
	m_modelParent->rotate(10*dt,math::vector3d::YAxis, scene::TS_World);

	m_cameraController->Update(dt);

	float vol = m_bgmSnd->getVolume();
	if (vol < 1)
	{
		vol += 0.05 * dt;
		m_bgmSnd->setVolume(vol);
	}

	switch (m_state)
	{
	case mray::AugTel::ConnectingState::EConnecting:
		if (m_cameraController->IsDone() && m_currItem < m_descItems.size())
		{
			m_currItem++;
			if (m_currItem == m_descItems.size())
			{
				m_state = EConnected;
				m_timeOut = 3;
				m_connectScreen->SetStatus(core::UTFString( "Connected Successfully!"), false);

			}
			else
			{
				m_connectScreen->SetStatus(m_descItems[m_currItem]->GetDescription(),true);
				m_cameraController->SetTarget(m_descItems[m_currItem]->GetSceneNode()->getAbsolutePosition());
				m_soundManager->playSound("sounds/robot.mp3", 0, true, 100, false, sound::ESNDT_2D);
			}
		}
		break;
	case mray::AugTel::ConnectingState::EConnected:
		if (m_timeOut <= 0)
			m_state = EDone;
		m_timeOut -= dt;
			
		break;
	case mray::AugTel::ConnectingState::EDone:
		m_exitCode = STATE_EXIT_CODE;
		break;
	default:
		break;
	}
}



void ConnectingState::LoadFromXML(xml::XMLElement* e)
{
}

void ConnectingState::OnAddGameEntity(game::GameEntityManager*m, game::GameEntity*e)
{
	const std::list<IObjectComponent*>&  lst = e->GetComponentsList();
	std::list<IObjectComponent*>::const_iterator it = lst.begin();
	for (; it != lst.end();++it)
	{
		IObjectComponent* c = *it;
		if (dynamic_cast<game::VisItemDescComponent*>(c))
		{
			m_descItems.push_back(dynamic_cast<game::VisItemDescComponent*>( c));
		}
		if (dynamic_cast<game::SceneComponent*>(c))
		{
			scene::ISceneNode*node= dynamic_cast<game::SceneComponent*>(c)->GetSceneNode();
			m_modelParent->addChild(node);
		}
	}

}


}
}

