
#include "stdafx.h"
#include "DepthVisualizer.h"
#include "DepthFrame.h"



namespace mray
{
namespace TBee
{
	DepthVisualizer::DepthVisualizer()
	{
		m_frame = 0;
	}
	DepthVisualizer::~DepthVisualizer()
	{}

	void DepthVisualizer::Init()
	{

		m_tex= Engine::getInstance().getDevice()->createEmptyTexture2D(false);
		m_tex->setMipmapsFilter(false);
		m_viewDepth = false;
	}


	void DepthVisualizer::Update()
	{
		if (!m_frame)
			return;
		if (m_depthData.Size.x != m_frame->GetSize().x
			|| m_depthData.Size.y != m_frame->GetSize().y)
		{
			m_depthData.createData(math::vector3d(m_frame->GetSize().x, m_frame->GetSize().y, 1), video::EPixel_R8G8B8);
			m_tex->createTexture(math::vector3di(m_frame->GetSize().x, m_frame->GetSize().y, 1), video::EPixel_R8G8B8);
		}


		math::vector3d* normals = m_frame->GetNormals();
		float *depth = m_frame->GetDepth();
		uchar* ptr = (uchar*)m_depthData.imageData;

		if (!depth)
			return;

		for (int y = 0; y < m_depthData.Size.y; ++y)
		{
			for (int x = 0; x < m_depthData.Size.x; ++x)
			{
				if (!m_viewDepth && normals)
				{
					(*ptr++) = (uchar)(math::Max<float>(0, (*normals).x*0.5f + 0.5f) * 255);
					(*ptr++) = (uchar)(math::Max<float>(0, (*normals).y*0.5f + 0.5f) * 255);
					(*ptr++) = (uchar)(math::Max<float>(0, (*normals).z*0.5f + 0.5f) * 255);
					++normals;
				}
				else
				{
					float v = *depth;
					v -= m_frame->GetMinThreshold();
					v /= m_frame->GetMaxThreshold();
					v *= 255;
					(*ptr++) = (uchar)v;
					(*ptr++) = (uchar)v;
					(*ptr++) = (uchar)v;

					++depth;
				}
			}
		}


		video::LockedPixelBox bb;
		bb.box.MinP = 0;
		bb.box.MaxP = m_depthData.Size;
		bb.format = m_depthData.format;
		bb.data = m_depthData.imageData;
		bb.rowPitch = m_depthData.Size.x;
		bb.slicePitch = m_depthData.Size.x*m_depthData.Size.y;

		video::IHardwarePixelBuffer* buff = m_tex->getSurface(0);
		buff->blitFromMemory(bb);
	}


}
}


