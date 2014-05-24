

#include "stdafx.h"
#include "GstFileVideoSource.h"
#include "ICameraVideoGrabber.h"
#include "GStreamVideoProvider.h"



namespace mray
{
namespace TBee
{

GstFileVideoSource::GstFileVideoSource(const core::string&left, const core::string&right)
{
	m_providers = new GStreamVideoProvider();
	SetVideo(left,right);
}

GstFileVideoSource::~GstFileVideoSource()
{
	m_providers->Disconnect();
	delete m_providers;
}

void GstFileVideoSource::Init()
{
	m_videoTex = Engine::getInstance().getDevice()->createTexture2D(4, video::EPixel_R8G8B8, true);
	m_videoTex->setMipmapsFilter(false);
}
void GstFileVideoSource::Open()
{
	m_providers->LoadSideBySideMovie(m_left,m_right);
}
void GstFileVideoSource::Close()
{
	m_providers->Disconnect();

}
bool GstFileVideoSource::Blit()
{

	bool dirty = false;
	m_providers->Update(gEngine.getFPS()->dt());
	if (m_providers->HasNewImage(0))
	{
		const video::ImageInfo* image = m_providers->GetImage(0);
		if (m_videoTex->getSize() != image->Size)
			dirty = true;
		m_videoTex->createTexture(math::vector3d(image->Size.x, image->Size.y, 1), image->format);
		video::LockedPixelBox box(math::box3d(0, image->Size), image->format, image->imageData);
		m_videoTex->getSurface(0)->blitFromMemory(box);
	}
	if (dirty)
		return true;
	return false;
}

math::vector2d GstFileVideoSource::GetEyeResolution(int i)
{
	math::vector3di sz = m_videoTex->getSize();
	return math::vector2d(sz.x/2 , sz.y);
}
video::ITexturePtr GstFileVideoSource::GetEyeTexture(int i)
{
	return m_videoTex;
}


void GstFileVideoSource::LoadFromXML(xml::XMLElement* e)
{

	xml::XMLAttribute* attr;

	attr = e->getAttribute("LeftVideo");
	if (attr)
		m_left = attr->value;
	attr = e->getAttribute("RightVideo");
	if (attr)
		m_right = attr->value;
	attr = e->getAttribute("FrameSize");
	if (attr)
	{
		math::vector2d sz = core::StringConverter::toVector2d(attr->value);
		m_providers->SetFrameSize(sz.x,sz.y);
	}


}

}
}



