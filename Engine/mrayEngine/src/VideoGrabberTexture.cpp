


#include "stdafx.h"
#include "VideoGrabberTexture.h"


namespace mray
{
namespace video
{


VideoGrabberTexture::VideoGrabberTexture()
{
}

VideoGrabberTexture::~VideoGrabberTexture()
{
}


void VideoGrabberTexture::Set(const GCPtr<IVideoGrabber>& grabber,ITextureCRef tex)
{
	m_grabber=grabber;
	m_texture=tex;
}


bool VideoGrabberTexture::Blit()
{
	if(!m_texture || !m_grabber )
		return false;
	if(!m_grabber->GrabFrame())
		return false;
	if(m_texture->getSize().x!=m_grabber->GetFrameSize().x ||
		m_texture->getSize().y!=m_grabber->GetFrameSize().y )
	{
		m_texture->setMipmapsFilter(false);
		m_texture->createTexture(math::vector3d(m_grabber->GetFrameSize().x,m_grabber->GetFrameSize().y,1),m_grabber->GetImageFormat());
	}
	const video::ImageInfo* ifo= m_grabber->GetLastFrame();

	video::LockedPixelBox box(math::box3d(0,ifo->Size),ifo->format,ifo->imageData);
	m_texture->getSurface(0)->blitFromMemory(box);
	return true;
}

}
}
