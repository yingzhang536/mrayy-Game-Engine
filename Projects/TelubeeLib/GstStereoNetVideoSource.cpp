

#include "stdafx.h"
#include "GstStereoNetVideoSource.h"
#include "ICameraVideoGrabber.h"
#include "GStreamVideoProvider.h"



namespace mray
{
namespace TBee
{

GstStereoNetVideoSource::GstStereoNetVideoSource(const core::string& ip )
{
	m_providers = new GStreamVideoProvider();
	SetIP(ip);
}

GstStereoNetVideoSource::~GstStereoNetVideoSource()
{
	m_providers->Disconnect();
	delete m_providers;
}

void GstStereoNetVideoSource::Init()
{
	m_remoteTex = Engine::getInstance().getDevice()->createTexture2D(4, video::EPixel_R8G8B8, true);
	m_remoteTex->setMipmapsFilter(false);
}
void GstStereoNetVideoSource::Open()
{
	m_providers->ConnectToCameras(m_ip, 5000, 5002, 5001);
}
void GstStereoNetVideoSource::Close()
{
	m_providers->Disconnect();

}
bool GstStereoNetVideoSource::Blit()
{

	bool dirty = false;
	m_providers->Update(gEngine.getFPS()->dt());
	if (m_providers->HasNewImage(0))
	{
		const video::ImageInfo* image = m_providers->GetImage(0);
		if (m_remoteTex->getSize() != image->Size)
			dirty = true;
		m_remoteTex->createTexture(math::vector3d(image->Size.x, image->Size.y, 1), image->format);
		video::LockedPixelBox box(math::box3d(0, image->Size), image->format, image->imageData);
		m_remoteTex->getSurface(0)->blitFromMemory(box);
	}
	if (dirty)
		return true;
	return false;
}

math::vector2d GstStereoNetVideoSource::GetEyeResolution(int i)
{
	math::vector3di sz = m_remoteTex->getSize();
	return math::vector2d(sz.x / 2, sz.y);
}
video::ITexturePtr GstStereoNetVideoSource::GetEyeTexture(int i)
{
	return m_remoteTex;
}

}
}

