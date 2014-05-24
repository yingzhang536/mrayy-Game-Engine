

#include "stdafx.h"
#include "GstSingleNetVideoSource.h"
#include "ICameraVideoGrabber.h"
#include "GStreamVideoProvider.h"



namespace mray
{
namespace TBee
{

GstSingleNetVideoSource::GstSingleNetVideoSource(const core::string& ip,int port )
{
	m_providers = new GStreamVideoProvider();
	SetIP(ip);
	SetPort(port);
}

GstSingleNetVideoSource::~GstSingleNetVideoSource()
{
	m_providers->Disconnect();
	delete m_providers;
}

void GstSingleNetVideoSource::Init()
{
	m_remoteTex = Engine::getInstance().getDevice()->createTexture2D(4, video::EPixel_R8G8B8, true);
	m_remoteTex->setMipmapsFilter(false);
}
void GstSingleNetVideoSource::Open()
{
	m_providers->ConnectToCameras(m_ip, m_port, 0, 0);
}
void GstSingleNetVideoSource::Close()
{
	m_providers->Disconnect();

}
bool GstSingleNetVideoSource::Blit()
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

math::vector2d GstSingleNetVideoSource::GetEyeResolution(int i)
{
	math::vector3di sz = m_remoteTex->getSize();
	return math::vector2d(sz.x , sz.y);
}
video::ITexturePtr GstSingleNetVideoSource::GetEyeTexture(int i)
{
	return m_remoteTex;
}

}
}

