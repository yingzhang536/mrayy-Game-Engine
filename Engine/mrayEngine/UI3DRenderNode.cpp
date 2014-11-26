

#include "stdafx.h"
#include "UI3DRenderNode.h"
#include "IGUIManager.h"
#include "MeshBufferData.h"
#include "RenderMaterial.h"
#include "RenderTechnique.h"
#include "RenderPass.h"
#include "Engine.h"

namespace mray
{
namespace scene
{


	IMPLEMENT_RTTI(UI3DRenderNode, MeshRenderableNode);

UI3DRenderNode::UI3DRenderNode()
	:MeshRenderableNode(0), m_UISize(256)
{
	m_UIManager = 0;
	m_UITexture = gEngine.getDevice()->createEmptyTexture2D(false);
	m_UITexture->setMipmapsFilter(false);
	m_UITexture->createTexture(math::vector3d(m_UISize.x, m_UISize.y, 1), video::EPixel_R8G8B8A8);
	m_UIRenderTarget = gEngine.getDevice()->createRenderTarget("", m_UITexture, 0, 0, 0);

	_createMesh();
}

UI3DRenderNode::~UI3DRenderNode()
{

}

void UI3DRenderNode::_createMesh()
{

	scene::MeshBufferData* bdata = getMesh()->addNewBuffer();
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
		math::vector2d(1, 1),
		math::vector2d(0, 1),
		math::vector2d(0, 0),
		math::vector2d(1, 0),
	};

	math::matrix3x3 rotMat;

	for (int j = 0; j < 4; ++j)
		tcPtr[j] = (rotMat*(tcPtr[j] * 2 - 1))*0.5 - 0.5f;
	ushort idxPtr[6] = { 0, 1, 2, 0, 2, 3 };

	pos->writeData(0, 4 * sizeof(math::vector3d), posPtr, true);
	tc->writeData(0, 4 * sizeof(math::vector2d), tcPtr, true);
	idx->writeData(0, 6 * sizeof(ushort), idxPtr, true);

	video::RenderMaterialPtr mtrl = new video::RenderMaterial();
	m_mtrl  = mtrl->CreateTechnique("Default")->CreatePass("Default");
	bdata->setMaterial(mtrl);

	m_mtrl->setTexture(m_UITexture, 0);

	m_mtrl->setRenderState(video::RS_Lighting, video::ES_DontUse);
	//m_mtrl->setRenderState(video::RS_ZWrite, video::ES_DontUse);
	//m_mtrl->setRenderState(video::RS_ZTest, video::ES_DontUse);
	m_mtrl->setRenderState(video::RS_CullFace, video::ES_DontUse);
	//m_mtrl->setMaterialRenderer(video::MRT_TRANSPARENT);
}

void UI3DRenderNode::SetUISize(const math::vector2d& sz)
{
	m_UISize = sz;
	m_UIRenderTarget->Resize(sz.x, sz.y);
}


void UI3DRenderNode::SetUIManager(GUI::IGUIManager* m)
{
	m_UIManager = m;
}

bool UI3DRenderNode::preRender(IRenderPass*pass)
{
	m_updated = false;
	return MeshRenderableNode::preRender(pass);
}

bool UI3DRenderNode::render(IRenderPass*pass)
{
	return MeshRenderableNode::render(pass);
}

void UI3DRenderNode::update(float dt)
{
	if (m_UIManager)
	{
		m_UIManager->Update(dt);
		if (!m_updated)
		{
			video::IVideoDevice*dev = gEngine.getDevice();

			video::IRenderTargetPtr oldRt = dev->getRenderTarget();
			dev->setRenderTarget(m_UIRenderTarget, true, true, video::SColor(0, 0, 0, 0));
			m_UIManager->DrawAll(&math::rectf(0, m_UIRenderTarget->GetSize()));

			dev->setRenderTarget(oldRt, false, false);



			m_updated = true;
		}
	}
	
	MeshRenderableNode::update(dt);
}

}
}



