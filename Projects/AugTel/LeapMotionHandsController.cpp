

#include "stdafx.h"
#include "LeapMotionHandsController.h"
#include "LeapMotionController.h"
#include "ParsedShaderPP.h"
#include "TextureRTWrap.h"
#include "AugTelSceneContext.h"
#include "HeadMount.h"
#include "SceneComponent.h"
#include "CameraComponent.h"
#include "HeadCameraComponent.h"
#include "BoneComponent.h"
#include "MeshBufferData.h"
#include "RenderTechnique.h"
#include "RenderPass.h"
#include "LeapMotionImageRetrival.h"


namespace mray
{
namespace AugTel
{

	class LeapMotionHandsControllerImpl:public Leap::Listener
	{
	public:

		 const int DEFAULT_TEXTURE_WIDTH = 640;
		 const int DEFAULT_TEXTURE_HEIGHT = 240;
		 const int DEFAULT_DISTORTION_WIDTH = 64;
		 const int DEFAULT_DISTORTION_HEIGHT = 64;


		LeapMotionController* m_controller;
		

		AugTelSceneContext* context;
		LeapMotionImageRetrival* images[2];

		core::string m_model;

		std::vector<game::GameEntity*> entLst;

		bool enabled;
		scene::MeshRenderableNode* m_surface[2];
		scene::ISceneNode* m_screenNode[2];
		video::RenderPass* m_screenMtrl[2];

		LeapMotionHandsControllerImpl()
		{
			enabled = true;
			m_controller = new LeapMotionController();
			images[0] = new LeapMotionImageRetrival(1);
			images[1] = new LeapMotionImageRetrival(0);
		}
		~LeapMotionHandsControllerImpl()
		{
			delete m_controller;
			delete images[0];
			delete images[1];
		}

		void setEnabled(bool e)
		{
			enabled = e;
			if (enabled)
			{
				m_controller->AddListener(this);
			}
			else
			{
				m_controller->RemoveListener(this);
			}
			for (int i = 0; i < entLst.size(); ++i)
			{
				entLst[i]->SetEnabled(e);
			}

			m_screenNode[0]->setVisible(e,true);
			m_screenNode[1]->setVisible(e, true);
		}

		void GenerateSurface()
		{
			int segments = 1;
			int vertCount = (segments + 1)*(segments + 1);
			int indCount = 6 * segments*segments;

			for (int i = 0; i < 2; ++i)
			{
				if (m_surface[i])
				{
					m_screenNode[i]->RemoveNode(m_surface[i]);
					m_surface[i] = 0;
				}
			}

			//Create Screen Node
			for (int i = 0; i < 2; ++i)
			{
				m_screenNode[i] = context->entManager->GetSceneManager()->createSceneNode("ScreenNode");
				scene::MeshRenderableNode* rnode = new scene::MeshRenderableNode(new scene::SMesh());
				m_surface[i] = rnode;

				scene::MeshBufferData* bdata = rnode->getMesh()->addNewBuffer();
				scene::IMeshBuffer* buffer = bdata->getMeshBuffer();

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
				math::vector2d tcPtr[4] = {
					math::vector2d(0, 1),
					math::vector2d(1, 1),
					math::vector2d(1, 0),
					math::vector2d(0, 0),
				};

				math::matrix3x3 rotMat;

				for (int j = 0; j < 4; ++j)
					tcPtr[j] = (rotMat*(tcPtr[j] * 2 - 1))*0.5 - 0.5f;
				ushort idxPtr[6] = { 0, 1, 2, 0, 2, 3 };

				pos->writeData(0, 4 * sizeof(math::vector3d), posPtr, true);
				tc->writeData(0, 4 * sizeof(math::vector2d), tcPtr, true);
				idx->writeData(0, 6 * sizeof(ushort), idxPtr, true);

				video::RenderMaterialPtr mtrl = new video::RenderMaterial();
				m_screenMtrl[i] = mtrl->CreateTechnique("Default")->CreatePass("ScreenPass");
				bdata->setMaterial(mtrl);

// 				rnode->SetHasCustomRenderGroup(true);
// 				rnode->SetTargetRenderGroup(scene::RGH_Solid - 5);

				m_screenMtrl[i]->setRenderState(video::RS_Lighting, video::ES_DontUse);
				m_screenMtrl[i]->setRenderState(video::RS_ZWrite, video::ES_DontUse);
				m_screenMtrl[i]->setRenderState(video::RS_ZTest, video::ES_DontUse);
				m_screenMtrl[i]->setRenderState(video::RS_CullFace, video::ES_DontUse);
				//m_screenMtrl[i]->setMaterialRenderer(video::MRT_TRANSPARENT);
				m_screenMtrl[i]->SetAlphaFunction(video::EAF_GreaterEqual);
				m_screenMtrl[i]->SetAlphaReferenceValue(0.5);

				m_screenNode[i]->AttachNode(rnode);
				//m_headNode->addChild(m_screenNode[i]);
				m_screenNode[i]->setPosition(math::vector3d(0, 0, 1));
				m_screenNode[i]->setScale(math::vector3d(3,1.9,1));				
				m_screenNode[i]->setVisible(false);
				m_screenNode[i]->setCullingType(scene::SCT_NONE);
			}

			context->headNode->GetLeftCamera()->addChild(m_screenNode[0]);
			context->headNode->GetRightCamera()->addChild(m_screenNode[1]);


		}
		void _loadHands()
		{

			context->entManager->loadFromFile(m_model, &entLst);
			GenerateSurface();
			return;
			game::GameEntity* ent = entLst[0];
			AugTel::HeadCameraComponent* eyes = ent->RetriveComponent<AugTel::HeadCameraComponent>(ent, "Head");
			VT::CameraComponent* cameraComponent = ent->RetriveComponent<VT::CameraComponent>(ent, "stereoCamera");
			

			if (eyes)
			{
				eyes->GetNode()->addChild(context->headNode);
				context->headNode->setPosition(eyes->GetOffset());
			}
			else if (cameraComponent)
			{
				cameraComponent->MountCamera(context->headNode, 0);
			}

		}


		void Init(AugTelSceneContext* c)
		{
			context = c;
			m_controller->Init();
			m_controller->GetController()->setPolicyFlags(Leap::Controller::POLICY_IMAGES);

			_loadHands();
		}

		void Start()
		{
			if (enabled)
				m_controller->AddListener(this);
		}
		void End()
		{
			m_controller->RemoveListener(this);
		}

		void OnRender(const math::rectf& rc, TBee::ETargetEye eye)
		{
			if (!enabled)
				return;
			Leap::Frame frame = m_controller->GetController()->frame();

			if (images[eye]->Capture(frame))
			{
				float palmY = frame.hands().rightmost().palmPosition().y;
//  				m_screenNode[eye]->setPosition(math::vector3d(0, 0, palmY*0.01));
//  				m_screenNode[eye]->setScale(math::vector3d(palmY, palmY, palmY)*0.01);

			}
			m_screenMtrl[eye]->setTexture(images[eye]->GetResult(), 0);


			m_screenNode[eye]->setVisible(true);
			m_screenNode[1-eye]->setVisible(false);
		}

		void Update(float dt)
		{
			if (!enabled)
				return;
			Leap::Frame frame = m_controller->GetController()->frame();
			frame.hand(0).confidence();
		}
	};

LeapMotionHandsController::LeapMotionHandsController()
{
	m_data = new LeapMotionHandsControllerImpl();
	m_enabled = true;
}
LeapMotionHandsController::~LeapMotionHandsController()
{
	delete m_data;
}


void LeapMotionHandsController::Init(AugTelSceneContext* context)
{
	m_data->Init(context);
}

void LeapMotionHandsController::Start(AugTelSceneContext* context)
{
	m_data->Start();
}

void LeapMotionHandsController::End(AugTelSceneContext* context)
{
	m_data->End();
}

void LeapMotionHandsController::Update(float dt)
{
	if (!m_enabled)
		return;

	m_data->Update(dt);

	controllers::IKeyboardController* kb = gAppData.inputMngr->getKeyboard();

	math::vector3d s;
	if (kb->getKeyState(KEY_LCONTROL))
	{
		s.x = (kb->getKeyState(KEY_LEFT) - kb->getKeyState(KEY_RIGHT))*dt*0.5f;
		s.y = (kb->getKeyState(KEY_UP) - kb->getKeyState(KEY_DOWN))*dt*0.5f;

		m_data->m_screenNode[0]->setScale(m_data->m_screenNode[0]->getScale() + s);
		m_data->m_screenNode[1]->setScale(m_data->m_screenNode[0]->getScale() + s);
	}
}

void LeapMotionHandsController::RenderStart(const math::rectf& rc, TBee::ETargetEye eye)
{
	if (!m_enabled)
		return;
	m_data->OnRender(rc, eye);

}

void LeapMotionHandsController::DebugRender(const math::rectf& rc, TBee::ETargetEye eye)
{
	if (!m_enabled)
		return;
	video::TextureUnit tu;
	tu.SetTexture(m_data->images[eye]->GetResult());
	gEngine.getDevice()->useTexture(0,&tu);
	//gEngine.getDevice()->draw2DImage(rc, 1);

	gEngine.getDevice()->useTexture(0,0);
}


math::vector3d LeapMotionHandsController::GetHandPosition(EHandType hand)
{
	return 0;
}

math::quaternion LeapMotionHandsController::GetHandRotation(EHandType hand)
{
	return 0;
}

math::vector3d LeapMotionHandsController::GetFingerPosition(EHandType hand, EFingerType finger)
{
	return 0;
}


void LeapMotionHandsController::LoadFromXML(xml::XMLElement* e)
{
	m_data->m_model = e->getValueString("Model");

}


void LeapMotionHandsController::SetEnabled(bool e)
{
	m_enabled = e;
	m_data->setEnabled(e);
}

}
}



