

#include "stdafx.h"
#include "GstNetVideoSource.h"
#include "ICameraVideoGrabber.h"
#include "GStreamVideoProvider.h"



namespace mray
{
namespace TBee
{

GstNetVideoSource::GstNetVideoSource(const core::string& ip )
{
	m_providers = new GStreamVideoProvider();
	SetIP(ip);
}

GstNetVideoSource::~GstNetVideoSource()
{
	m_providers->Disconnect();
	delete m_providers;
}

void GstNetVideoSource::Init()
{
	m_remoteTex = Engine::getInstance().getDevice()->createTexture2D(4, video::EPixel_R8G8B8, true);
	m_remoteTex->setMipmapsFilter(false);
}
void GstNetVideoSource::Open()
{
	m_providers->ConnectToCameras(m_ip, 5000, 5002, 5001);
}
void GstNetVideoSource::Close()
{
	m_providers->Disconnect();

}
bool GstNetVideoSource::Blit()
{

	bool dirty = false;
	m_providers->Update(gFPS.dt());
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

math::vector2d GstNetVideoSource::GetEyeResolution(int i)
{
	math::vector3di sz = m_remoteTex->getSize();
	return math::vector2d(sz.x / 2, sz.y);
}
video::ITexturePtr GstNetVideoSource::GetEyeTexture(int i)
{
	return m_remoteTex;
}

}
}

