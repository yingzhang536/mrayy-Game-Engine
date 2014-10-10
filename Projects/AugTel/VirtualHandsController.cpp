

#include "stdafx.h"
#include "VirtualHandsController.h"
#include "AugTelSceneContext.h"
#include "ShaderResourceManager.h"
#include "HeadMount.h"
#include "RobotCommunicatorComponent.h"
#include "ParsedShaderPP.h"
#include "SceneComponent.h"
#include "HeadCameraComponent.h"

#include "RenderMaterial.h"
#include "RenderTechnique.h"
#include "RenderPass.h"
#include "CameraComponent.h"
#include "TextureResourceManager.h"
#include "ICameraVideoSource.h"
#include "TextureRTWrap.h"



namespace mray
{
namespace AugTel
{

	class VirtualHandsStateImpl
	{

	protected:

		void GenerateBRDF()
		{
			BRDFTexture = gEngine.getDevice()->createEmptyTexture2D(false);
			BRDFTexture->setMipmapsFilter(false);
			BRDFTexture->createTexture(math::vector3d(512, 512, 1), video::EPixel_Float16_R);
			video::IRenderTargetPtr rt = gEngine.getDevice()->createRenderTarget("", BRDFTexture, 0, 0, 0);

			video::IGPUShaderProgramPtr BRDF_FP = gShaderResourceManager.loadShaderFromFile("BRDF_PreComp.cg", video::EShader_FragmentProgram, "main_fp_PHBeckmann", video::ShaderPredefList(), "cg");
			gEngine.getDevice()->set2DMode();
			gEngine.getDevice()->setRenderTarget(rt);
			gEngine.getDevice()->setFragmentShader(BRDF_FP);
			gEngine.getDevice()->draw2DImage(math::rectf(0, 0, 512, 512), 1);
			gEngine.getDevice()->setRenderTarget(0);
			gEngine.getDevice()->setFragmentShader(0);
			rt = 0;
		}
	public:
		video::ITexturePtr BRDFTexture;

		AugTelSceneContext* context;

		VT::RobotCommunicatorComponent* robotCommunicator;
		video::ParsedShaderPP* m_blurShader;
		std::vector<game::GameEntity*> entLst;
	public:
		VirtualHandsStateImpl()
		{
		}
		~VirtualHandsStateImpl()
		{
		}

		void Init()
		{
			video::ParsedShaderPP* pp = new video::ParsedShaderPP(gEngine.getDevice());
			pp->LoadXML(gFileSystem.openFile("blur.peff"));
			m_blurShader = pp;

			GenerateBRDF();
		}


		void _GenerateLightMap()
		{
			m_blurShader->Setup(math::rectf(0, 0, 256, 256));
			ulong currTime = gEngine.getTimer()->getMilliseconds();
			/*
			if (m_takeScreenShot)
			{

				core::string name = gFileSystem.getAppPath() + "screenShots/Image_";
				name += core::StringConverter::toString(currTime);
				name += ".png";
				gTextureResourceManager.writeResourceToDist(m_camVideoSrc->GetEyeTexture(0), name);
			}*/
			video::IRenderTarget* prevRT = gEngine.getDevice()->getRenderTarget();
			video::ITexture* t = context->videoSource->GetEyeTexture(0);
			for (int i = 0; i < 10; ++i)
			{
				m_blurShader->GetParam("Offset")->SetValue((i + 1) * 4);
				m_blurShader->render(&video::TextureRTWrap(t));
				t = m_blurShader->getOutput()->GetColorTexture();
				/*

				if (m_takeScreenShot)
				{
					time_t rawtime;
					struct tm  timeinfo;

					ulong currTime = gEngine.getTimer()->getMilliseconds();

					core::string name = gFileSystem.getAppPath() + "screenShots/ImageDownSample" + core::StringConverter::toString(i) + "_";
					name += core::StringConverter::toString(currTime);
					name += ".png";
					gTextureResourceManager.writeResourceToDist(m_blurShader->getOutput()->GetColorTexture(0), name);

				}*/
			}

			gEngine.getDevice()->setRenderTarget(prevRT, 0, 0);
		}

	};

VirtualHandsController::VirtualHandsController()
{
	m_data = new VirtualHandsStateImpl();
	m_lightMapTimer = 0;
	m_enabled = true;

}

VirtualHandsController::~VirtualHandsController()
{
	delete m_data;
}


void VirtualHandsController::Init(AugTelSceneContext* context)
{

	
	m_data->Init();
	m_data->context = context;

	context->entManager->loadFromFile(m_model, &m_data->entLst);
	game::SceneComponent* modelComp = game::IGameComponent::RetriveComponent<game::SceneComponent>(m_data->entLst[0], "SkinnedArms");
	if (modelComp)
	{
		for (int i = 0; i < modelComp->GetSceneNode()->GetAttachedNodesCount(); ++i)
		{
			scene::IRenderable* r = modelComp->GetSceneNode()->GetAttachedNode(i);
			r->getMaterial(0)->GetTechniqueAt(0)->GetPassAt(0)->setTexture(m_data->m_blurShader->getOutput()->GetColorTexture(0), 3);
			//r->getMaterial(0)->GetTechniqueAt(0)->GetPassAt(0)->setTexture(m_data->BRDFTexture, 3);
		}
	}
	m_data->robotCommunicator = 0;
	const std::list<IObjectComponent*>& compLst = m_data->entLst[0]->GetComponent(VT::RobotCommunicatorComponent::getClassRTTI());
	if (compLst.size() != 0)
	{
		m_data->robotCommunicator = dynamic_cast<VT::RobotCommunicatorComponent*>(*compLst.begin());
		m_data->robotCommunicator->SetEnabled(false);
	}

	game::GameEntity* ent = m_data->entLst[0];
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

void VirtualHandsController::Start(AugTelSceneContext* context)
{

	m_data->robotCommunicator->SetEnabled(true);
}

void VirtualHandsController::End(AugTelSceneContext* context)
{

	m_data->robotCommunicator->SetEnabled(false);
}

void VirtualHandsController::Update(float dt)
{
}
void VirtualHandsController::RenderStart(const math::rectf& rc, TBee::ETargetEye eye)
{

	if (gEngine.getTimer()->getSeconds() - m_lightMapTimer > 200)
	{
		m_data->_GenerateLightMap();
		m_lightMapTimer = gEngine.getTimer()->getSeconds();
	}


}

void VirtualHandsController::DebugRender(const math::rectf& rc, TBee::ETargetEye eye)
{

}


math::vector3d VirtualHandsController::GetHandPosition(EHandType hand)
{
	return 0;

}

math::quaternion VirtualHandsController::GetHandRotation(EHandType hand)
{
	return 0;
}

math::vector3d VirtualHandsController::GetFingerPosition(EHandType hand, EFingerType finger)
{
	return 0;
}


void VirtualHandsController::LoadFromXML(xml::XMLElement* e)
{
	m_model = e->getValueString("Model");
}

void VirtualHandsController::SetEnabled(bool e)
{
	m_enabled = e;
	for (int i = 0; i < m_data->entLst.size(); ++i)
	{
		m_data->entLst[i]->SetEnabled(e);
	}
}

}
}



