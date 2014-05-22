

#include "stdafx.h"
#include "GstStereoStreamVideoSource.h"
#include "GstSingleNetVideoSource.h"
#include "GStreamVideoProvider.h"




namespace mray
{
namespace TBee
{

GstStereoStreamVideoSource::GstStereoStreamVideoSource(int port1 , int port2 )
{
	m_providers = new GStreamVideoProvider();
	m_port1 = port1;
	m_port2 = port2;
}
GstStereoStreamVideoSource::~GstStereoStreamVideoSource()
{
	m_providers->Disconnect();
	delete m_providers;
}

void GstStereoStreamVideoSource::SetPorts(int port1, int port2)
{
	m_port1 = port1;
	m_port2 = port2;
}

void GstStereoStreamVideoSource::Init()
{
	m_remoteTex = Engine::getInstance().getDevice()->createTexture2D(4, video::EPixel_R8G8B8, true);
	m_remoteTex->setMipmapsFilter(false);
}
void GstStereoStreamVideoSource::Open()
{
	m_providers->Open2Streams(m_port1,m_port2);

}
void GstStereoStreamVideoSource::Close()
{
	m_providers->Disconnect();
}
bool GstStereoStreamVideoSource::Blit()
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

math::vector2d GstStereoStreamVideoSource::GetEyeResolution(int i)
{
	math::vector3di sz = m_remoteTex->getSize();
	return math::vector2d(sz.x /2, sz.y);
}
video::ITexturePtr GstStereoStreamVideoSource::GetEyeTexture(int i)
{
	return m_remoteTex;
}

}
}

