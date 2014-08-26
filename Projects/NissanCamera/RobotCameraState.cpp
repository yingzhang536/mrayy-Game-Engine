
#include "stdafx.h"
#include "RobotCameraState.h"
#include "LocalCameraVideoSource.h"
#include "NissanRobotCommunicator.h"
#include "CRobotConnector.h"

#include "MeshBufferData.h"
#include "RenderMaterial.h"
#include "RenderTechnique.h"
#include "RenderPass.h"
#include "GUIBatchRenderer.h"
#include "FontResourceManager.h"
#include "NCAppGlobals.h"
#include "OptiTrackHeadController.h"
#include "CalibHeadController.h"
#include "GUIThemeManager.h"
#include "GUIConsole.h"

namespace mray
{
namespace NCam
{

	class TestController :public TBee::IHeadController
	{
	protected:
	public:
		TestController()
		{}
		virtual~TestController()
		{}

		virtual math::quaternion GetHeadOrientation()
		{
			float time = gEngine.getTimer()->getSeconds();
			math::vector3d angles;
			//angles.x = 20 * sin(time*0.001f);
			//angles.y = 30 * sin(time*0.002f);
			angles.z = 20 * sin(time*0.005f);
			return math::quaternion(angles);
		}
		virtual math::vector3d GetHeadPosition()
		{
			return 0;
		}
	};

RobotCameraState::RobotCameraState()
:IRenderingState("RobotState")
{
	m_exitCode = 0;

	m_robotConnector = new TBee::CRobotConnector();
	m_videoSource = new TBee::LocalCameraVideoSource();
	m_robotComm = new NCam::NissanRobotCommunicator();
	m_robotConnector->SetCommunicator(m_robotComm);

	m_headController = new TBee::CalibHeadController(new TBee::OptiTrackHeadController(1));
	//m_headController = new TBee::CalibHeadController(new TestController());

	m_robotConnector->SetHeadController(m_headController);
	m_hmdFov = 80;

	GUI::GUIBatchRenderer*r = new GUI::GUIBatchRenderer();
	r->SetDevice(Engine::getInstance().getDevice());
	m_guiRenderer = r;
	m_lockAxis[0] = true;
	m_lockAxis[1] = false;
	m_lockAxis[2] = true;
	m_camConfigDirty = true;
	m_useLensCorrection = false;
	m_cameraConfiguration = 0;

	m_arServiceProvider = new ARServiceProvider();
	m_arServiceProvider->AddListener(this);

}

RobotCameraState::~RobotCameraState()
{
	delete m_videoSource;
}



void RobotCameraState::SetCameraInfo(TBee::ETargetEye eye, int id)
{
	((TBee::LocalCameraVideoSource*)m_videoSource)->SetCameraID(GetEyeIndex(eye), id);
}


void RobotCameraState::InitState()
{
	Parent::InitState();

	m_guimngr = new GUI::GUIManager(gEngine.getDevice());
	m_guimngr->SetActiveTheme(GUI::GUIThemeManager::getInstance().getActiveTheme());
	{
		GUI::IGUIPanelElement* m_guiroot;
		m_guiroot = (GUI::IGUIPanelElement*)new GUI::IGUIPanelElement(core::string(""), m_guimngr);
		m_guiroot->SetDocking(GUI::EED_Fill);
		m_guimngr->SetRootElement(m_guiroot);

		GUI::GUIConsole* console = new GUI::GUIConsole(m_guimngr);
		m_guiroot->AddElement(console);
		console->SetAnchorLeft(true);
		console->SetAnchorRight(true);
		console->SetAnchorTop(true);
		console->SetPosition(0);
		console->SetSize(math::vector2d(100, 300));

		m_console = console;
	}

	m_sceneManager = new scene::SceneManager(Engine::getInstance().getDevice());
	float cameraScreenDistance = 1.0f;
	m_videoSource->Init();

	m_lensCorrectionPP = new video::ParsedShaderPP(Engine::getInstance().getDevice());
	m_lensCorrectionPP->LoadXML(gFileSystem.openFile("LensCorrection.peff"));

	{
		scene::CameraNode* cam[2];
		video::EPixelFormat pf = video::EPixel_R8G8B8A8;//video::EPixel_R8G8B8A8;//
		m_headNode = m_sceneManager->createSceneNode();


		for (int i = 0; i < 2; ++i)
		{
			cam[i] = m_sceneManager->createCamera();
			m_viewport[i] = new scene::ViewPort("", cam[i], 0, 0, math::rectf(0, 0, 1, 1), 0);
			m_viewport[i]->SetClearColor(video::SColor(0, 0, 0, 1));

			video::ITexturePtr renderTargetTex = Engine::getInstance().getDevice()->createTexture2D(math::vector2d(1, 1), pf, true);
			renderTargetTex->setBilinearFilter(false);
			renderTargetTex->setTrilinearFilter(false);
			renderTargetTex->setMipmapsFilter(false);

			video::IRenderTargetPtr rt = Engine::getInstance().getDevice()->createRenderTarget(mT(""), renderTargetTex, video::IHardwareBufferPtr::Null, video::IHardwareBufferPtr::Null, false);
			m_viewport[i]->setRenderTarget(rt);
			m_viewport[i]->setOnlyToRenderTarget(true);
			m_viewport[i]->SetAutoUpdateRTSize(true);
			m_viewport[i]->AddListener(this);

			cam[i]->setZNear(0.01);
			cam[i]->setZFar(100);

			//if (ATAppGlobal::Instance()->oculusDevice)
			cam[i]->setFovY(m_hmdFov);
			cam[i]->setAutoUpdateAspect(true);
			m_camera[i] = cam[i];
			//m_headNode->addChild(cam[i]);
		}
		m_camera[GetEyeIndex(TBee::Eye_Left )]->setPosition(math::vector3d(-0.03, 0, 0));
		m_camera[GetEyeIndex(TBee::Eye_Right)]->setPosition(math::vector3d(+0.03, 0, 0));
	}
	//Create Screen Node
	for (int i = 0; i < 2;++i)
	{
		m_screenNode[i] = m_sceneManager->createSceneNode("ScreenNode");
		scene::MeshRenderableNode* rnode = new scene::MeshRenderableNode(new scene::SMesh());
	
		scene::MeshBufferData* bdata= rnode->getMesh()->addNewBuffer();
		scene::IMeshBuffer* buffer= bdata->getMeshBuffer();

		video::IHardwareStreamBuffer* pos = buffer->createStream(0, video::EMST_Position, video::ESDT_Point3f, 4, video::IHardwareBuffer::EUT_WriteOnly, false, false);
		video::IHardwareStreamBuffer* tc = buffer->createStream(0, video::EMST_Texcoord, video::ESDT_Point2f, 4, video::IHardwareBuffer::EUT_WriteOnly, false, false);
		video::IHardwareIndexBuffer* idx = buffer->createIndexBuffer(video::IHardwareIndexBuffer::EIT_16Bit, 6, video::IHardwareBuffer::EUT_WriteOnly, false, false);

		math::vector3d posPtr[4] =
		{
			math::vector3d(-1, -1, 0),
			math::vector3d(+1, -1, 0),
			math::vector3d(+1, +1, 0),
			math::vector3d(-1, +1, 0)
		};
		math::vector2d tcPtr[4]={
			 			math::vector2d(1,0),
			 			math::vector2d(0,0),
			 			math::vector2d(0,1),
			 			math::vector2d(1, 1)

		};

		math::matrix3x3 rotMat;


		if (m_cameraConfiguration->cameraRotation[i] != TBee::TelubeeCameraConfiguration::None)
		{
			if (m_cameraConfiguration->cameraRotation[i] == TBee::TelubeeCameraConfiguration::CW)
				rotMat.setAngle(90);
			else if (m_cameraConfiguration->cameraRotation[i] == TBee::TelubeeCameraConfiguration::CCW)
				rotMat.setAngle(-90);
			else if (m_cameraConfiguration->cameraRotation[i] == TBee::TelubeeCameraConfiguration::Flipped)
				rotMat.setAngle(180);
			//     		math::Swap(tc.ULPoint.x, tc.ULPoint.y);
			//     		math::Swap(tc.BRPoint.x, tc.BRPoint.y);

		}
		/*
		if (i == 0)
			rotMat.setAngle(90);
		else rotMat.setAngle(-90);
		*/
		for (int j = 0; j < 4; ++j)
			tcPtr[j] = (rotMat*(tcPtr[j] * 2 - 1))*0.5 - 0.5f;
		ushort idxPtr[6] = { 0, 1, 2, 0, 2, 3 };

		pos->writeData(0, 4 * sizeof(math::vector3d), posPtr, true);
		tc->writeData(0, 4 * sizeof(math::vector2d), tcPtr, true);
		idx->writeData(0, 6 * sizeof(ushort), idxPtr, true);

		video::RenderMaterialPtr mtrl = new video::RenderMaterial();
		m_screenMtrl[i] = mtrl->CreateTechnique("Default")->CreatePass("Default");
		bdata->setMaterial(mtrl);

		m_screenMtrl[i]->setRenderState(video::RS_Lighting, video::ES_DontUse);
		m_screenMtrl[i]->setRenderState(video::RS_CullFace, video::ES_DontUse);

		m_screenNode[i]->AttachNode(rnode);
		m_headNode->addChild(m_screenNode[i]);
		m_screenNode[i]->setPosition(math::vector3d(0, 0, cameraScreenDistance));
		m_screenNode[i]->setVisible(false);
	}

 	m_screenNode[GetEyeIndex(TBee::Eye_Left)]->setOrintation(math::quaternion(180, math::vector3d::ZAxis));
 	m_screenNode[GetEyeIndex(TBee::Eye_Right)]->setOrintation(math::quaternion(180, math::vector3d::ZAxis));


	m_console->AddToHistory("System Inited.", video::DefaultColors::Green);
}


bool RobotCameraState::OnEvent(Event* e, const math::rectf& rc)
{
	if (Parent::OnEvent(e, rc)  || m_guimngr->OnEvent(e,&rc))
		return true;
	bool ok = false;



	if (e->getType() == ET_Keyboard)
	{
		KeyboardEvent* evt = (KeyboardEvent*)e;
		if (evt->press)
		{
			if (evt->key == KEY_SPACE)
			{
				if (m_robotConnector->IsRobotConnected())
					m_robotConnector->EndUpdate();
				else
				{
					if (!m_robotConnector->IsRobotConnected())
						m_robotConnector->ConnectRobot();
					m_robotConnector->StartUpdate();
				}
				ok = true;
			}
			else if (evt->key == KEY_X)
			{
				m_lockAxis[0] = !m_lockAxis[0];
				ok = true;
			}
			else if (evt->key == KEY_Y)
			{
				m_lockAxis[1] = !m_lockAxis[1];
				ok = true;
			}
			else if (evt->key == KEY_Z)
			{
				m_lockAxis[2] = !m_lockAxis[2];
				ok = true;
			}
			else if (evt->key == KEY_C)
			{
				m_headController->Calibrate();
				ok = true;
			}
			else if (evt->key == KEY_H)
			{
				m_robotConnector->SetData("Homing", "", false);
				ok = true;
			}
		}
	}
	return ok;
}
void RobotCameraState::OnEnter(TBee::IRenderingState*prev)
{
	Parent::OnEnter(prev);
	m_videoSource->Open();
	//m_robotConnector->ConnectRobot();

}

void RobotCameraState::OnExit()
{
	Parent::OnExit();
	m_videoSource->Close();
	m_robotConnector->DisconnectRobot();
}

float time = 0;
void RobotCameraState::Update(float dt)
{
	Parent::Update(dt);
	m_videoSource->Blit();
	m_guimngr->Update(dt);
	m_robotConnector->UpdateStatus();
	math::vector3d r = m_robotConnector->GetHeadRotation();
	math::vector3d pos = m_robotConnector->GetHeadPosition();

	math::vector3d rot = r;
	rot.x = r.y;
	rot.z = r.x;
	rot.y = r.z;
// 	rot.x = -rot.x;
// 	rot.z = -rot.z;
	if (m_lockAxis[0])
		rot.x = 0;
	if (m_lockAxis[1])
		rot.y = 0;
	if (m_lockAxis[2])
		rot.z = 0;
	m_headNode->setOrintation(rot);

	m_headNode->setPosition(pos);
}


class TextureRenderTarget :public video::IRenderArea
{
protected:
	video::ITexturePtr m_tex;
public:
	TextureRenderTarget(video::ITexturePtr tex){ m_tex = tex; }
	virtual~TextureRenderTarget()
	{
	}
	virtual const video::ITexturePtr& GetColorTexture(int i = 0) { return m_tex; }
	virtual int GetColorTextureCount() { return 1; }
	virtual void Resize(int x, int y) {}
	virtual math::vector2di GetSize()
	{
		return math::vector2di(m_tex->getSize().x, m_tex->getSize().y);
	}

};


video::IRenderTarget* RobotCameraState::Render(const math::rectf& rc, TBee::ETargetEye eye)
{
	video::IVideoDevice* device = Engine::getInstance().getDevice();
	int index = GetEyeIndex(eye);
	video::ITexture* camTex = m_videoSource->GetEyeTexture(index);



	math::vector3d scale;
	scale.x = (float)camTex->getSize().x / (float)camTex->getSize().y;
	scale.y = 1;
	scale.z = 1;
	if (m_useLensCorrection )
	{
		math::vector2d size(rc.getSize());
		video::ParsedShaderPP::MappedParams* texRect = m_lensCorrectionPP->GetParam("texRect");

		if (texRect)
		{
			math::rectf r = m_videoSource->GetEyeTexCoords(index);
			texRect->SetValue(math::vector4d(r.ULPoint.x, r.ULPoint.y, r.BRPoint.x, r.BRPoint.y));
		}
		if (m_camConfigDirty)
		{
			video::ParsedShaderPP::MappedParams* OpticalCenter = m_lensCorrectionPP->GetParam("OpticalCenter");
			video::ParsedShaderPP::MappedParams* FocalCoeff = m_lensCorrectionPP->GetParam("FCoff");
			video::ParsedShaderPP::MappedParams* KPCoeff = m_lensCorrectionPP->GetParam("KCoff");

			if (OpticalCenter)
				OpticalCenter->SetValue(m_cameraConfiguration->OpticalCenter);
			if (FocalCoeff)
				FocalCoeff->SetValue(m_cameraConfiguration->FocalCoeff);
			if (KPCoeff)
				KPCoeff->SetValue(m_cameraConfiguration->KPCoeff);
		}

		m_lensCorrectionPP->Setup(math::rectf(0, size));
		m_lensCorrectionPP->render(&TextureRenderTarget(camTex));
		camTex = m_lensCorrectionPP->getOutput()->GetColorTexture();
	}
	m_screenNode[index]->setVisible(true);
	m_screenNode[index]->setScale(scale);
	m_screenMtrl[index]->setTexture(camTex, 0);
	m_viewport[index]->setAbsViewPort(rc);
	m_viewport[index]->draw();
	Parent::Render(rc, eye);
	video::TextureUnit tex;

	device->setRenderTarget(m_renderTarget[index]);
	tex.SetTexture(m_viewport[index]->getRenderTarget()->GetColorTexture());
	device->useTexture(0, &tex);
	math::rectf tc = math::rectf(0, 0, 1, 1);
	device->draw2DImage(rc, 1, 0, &tc);
	m_screenNode[index]->setVisible(false);
	_RenderUI(rc);

	return m_renderTarget[index];
}

void RobotCameraState::_RenderUI(const math::rectf& rc)
{
	if (!TBee::AppData::Instance()->IsDebugging)
		return;
	GUI::IFont* font = gFontResourceManager.getDefaultFont();
	GUI::FontAttributes attr;


	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	TBee::AppData* app = TBee::AppData::Instance();
	m_guimngr->DrawAll(&rc);
	
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
		{
			core::string msg = "Axis Lock: ";
			if (m_lockAxis[0])
				msg += "[X]";
			else msg += "X";
			msg += ",";
			if (m_lockAxis[1])
				msg += "[Y]";
			else msg += "Y";
			msg += ",";
			if (m_lockAxis[2])
				msg += "[Z]";
			else msg += "Z";
			attr.fontColor.Set(0, 1, 0, 1);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
		}
		attr.fontColor.Set(1, 1, 1, 1);
		if (m_robotConnector->GetHeadController())
		{
			math::vector3d head;
			head = m_robotConnector->GetHeadRotation();
			core::string msg = mT("Head Rotation: ") + core::StringConverter::toString(head);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;

			head = m_robotConnector->GetHeadPosition();
			msg = mT("Head Position: ") + core::StringConverter::toString(head);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
		}
		else
		{
			core::string msg = mT("Head: Non");
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
		}
		if (m_robotConnector->GetRobotController())
		{

			math::vector2d speed;
			float rot;
			speed = m_robotConnector->GetSpeed();
			rot = m_robotConnector->GetRotation();
			core::string msg = mT("Robot Speed: ") + core::StringConverter::toString(speed);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
			msg = mT("Robot Rotation: ") + core::StringConverter::toString(rot);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;


		}


		core::string msg = mT("Is Homing: ");
		msg += m_robotComm->IsHoming() ? "Yes" : "No";
		font->print(r, &attr, 0, msg, m_guiRenderer);
		r.ULPoint.y += attr.fontSize + 5;
		m_guiRenderer->Flush();
	}

}

void RobotCameraState::LoadFromXML(xml::XMLElement* e)
{
	Parent::LoadFromXML(e);

	xml::XMLElement* c = e->getSubElement("Calibration");
	if (c)
		m_headController->LoadFromXML(c);


	core::string camConfigName = e->getValueString("CameraConfiguration");

	m_useLensCorrection = e->getValueBool("UseLensCorrection");

	m_cameraConfiguration = NCAppGlobals::Instance()->camConfig->GetCameraConfiguration(camConfigName);


	m_camConfigDirty = true;
}
xml::XMLElement* RobotCameraState::WriteToXML(xml::XMLElement* e)
{
	xml::XMLElement* elem = Parent::WriteToXML(e);

	m_headController->WriteToXML(elem);
	return elem;
}

void RobotCameraState::OnARContents(ARCommandAddData* cmd)
{
	cmd->group->objects;
}
void RobotCameraState::OnVechicleData()
{

}
void RobotCameraState::OnDeletedGroup(ARCommandDeleteGroup* cmd)
{

}

}
}
