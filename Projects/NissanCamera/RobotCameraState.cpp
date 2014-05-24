
#include "stdafx.h"
#include "RobotCameraState.h"
#include "LocalCameraVideoSource.h"
#include "LocalRobotCommunicator.h"
#include "CRobotConnector.h"

#include "MeshBufferData.h"
#include "RenderMaterial.h"
#include "RenderTechnique.h"
#include "RenderPass.h"
#include "GUIBatchRenderer.h"
#include "FontResourceManager.h"
#include "AppData.h"
#include "OptiTrackHeadController.h"
#include "CalibHeadController.h"

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
			float time = gTimer.getActualTimeAccurate();
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
{
	m_exitCode = 0;

	m_robotConnector = new TBee::CRobotConnector();
	m_videoSource = new TBee::LocalCameraVideoSource();
	m_robotConnector->SetCommunicator(new TBee::LocalRobotCommunicator());

	//m_headController = new TBee::CalibHeadController(new TBee::OptiTrackHeadController(1));
	m_headController = new TBee::CalibHeadController(new TestController());

	m_robotConnector->SetHeadController(m_headController);
	m_hmdFov = 80;

	GUI::GUIBatchRenderer*r = new GUI::GUIBatchRenderer();
	r->SetDevice(Engine::getInstance().getDevice());
	m_guiRenderer = r;
	m_lockAxis[0] = true;
	m_lockAxis[1] = false;
	m_lockAxis[2] = true;
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

	m_sceneManager = new scene::SceneManager(Engine::getInstance().getDevice());
	float cameraScreenDistance = 1.0f;
	m_videoSource->Init();

	{
		scene::CameraNode* cam[2];
		video::EPixelFormat pf = video::EPixel_R8G8B8A8;//video::EPixel_R8G8B8A8;//
		m_headNode = m_sceneManager->createSceneNode();


		for (int i = 0; i < 2; ++i)
		{
			cam[i] = m_sceneManager->createCamera();
			m_viewport[i] = new scene::ViewPort("", cam[i], 0, 0, math::rectf(0, 0, 1, 1), 0);
			m_viewport[i]->SetClearColor(video::SColor(1, 1, 1, 1));

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
		math::vector2d tcPtr[4] =
		{
			math::vector2d(1,0),
			math::vector2d(0,0),
			math::vector2d(0,1),
			math::vector2d(1,1)
		};
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
}


bool RobotCameraState::OnEvent(Event* e, const math::rectf& rc)
{
	if (Parent::OnEvent(e, rc))
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
					m_robotConnector->StartUpdate();
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
		}
	}
	return ok;
}
void RobotCameraState::OnEnter(TBee::IRenderingState*prev)
{
	Parent::OnEnter(prev);
	m_videoSource->Open();
	m_robotConnector->ConnectRobot();

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
	m_robotConnector->UpdateStatus();
	math::vector3d rot = m_robotConnector->GetHeadRotation();
	math::vector3d pos = m_robotConnector->GetHeadPosition();

	rot.x = 20 * sin(time);
	time += dt;
	rot.y = -rot.y;
	rot.z = -rot.z;
	if (m_lockAxis[0])
		rot.x = 0;
	if (m_lockAxis[1])
		rot.y = 0;
	if (m_lockAxis[2])
		rot.z = 0;
	m_headNode->setOrintation(rot);

	m_headNode->setPosition(pos);
}

video::IRenderTarget* RobotCameraState::Render(const math::rectf& rc, TBee::ETargetEye eye)
{
	video::IVideoDevice* device = Engine::getInstance().getDevice();
	int index = GetEyeIndex(eye);
	video::ITexture* camTex = m_videoSource->GetEyeTexture(index);
	math::vector3d scale;
	scale.x = (float)camTex->getSize().x / (float)camTex->getSize().y;
	scale.y = 1;
	scale.z = 1;
	m_screenNode[index]->setVisible(true);
	m_screenNode[index]->setScale(scale);
	m_screenMtrl[index]->setTexture(camTex, 0);
	m_viewport[index]->setAbsViewPort(rc);
	m_viewport[index]->draw();
	Parent::Render(rc, eye);
	device->setRenderTarget(m_renderTarget[index]);
	video::TextureUnit tex;


	tex.SetTexture(m_viewport[index]->getRenderOutput()->getColorTexture());
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
		m_guiRenderer->Flush();
	}

}

void RobotCameraState::LoadFromXML(xml::XMLElement* e)
{
	Parent::LoadFromXML(e);

	xml::XMLElement* c = e->getSubElement("Calibration");
	if (c)
		m_headController->LoadFromXML(c);
}
xml::XMLElement* RobotCameraState::WriteToXML(xml::XMLElement* e)
{
	xml::XMLElement* elem = Parent::WriteToXML(e);

	m_headController->WriteToXML(elem);
	return elem;
}


}
}
