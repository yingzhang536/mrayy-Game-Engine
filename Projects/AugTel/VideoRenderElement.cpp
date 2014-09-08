
#include "stdafx.h"
#include "VideoRenderElement.h"
#include "IVideoSource.h"
#include "VideoResourceManager.h"

namespace mray
{
namespace AugTel
{


VideoRenderElement::VideoRenderElement()
{
	m_video = new video::VideoGrabberTexture();
	m_texture = gEngine.getDevice()->createEmptyTexture2D(true);
	m_visible = true;

}
VideoRenderElement::~VideoRenderElement()
{
	delete m_video;

}

void VideoRenderElement::LoadVideo(const core::string&path)
{
	video::IVideoSourcePtr vdo = gVideoResourceManager.LoadVideo(path);
	m_video->Set(vdo, m_texture);
}

void VideoRenderElement::Start()
{

	m_visible = true;
	video::IVideoSource* v = (video::IVideoSource*)m_video->GetGrabber().pointer();

	if (v)
	{
		v->seek(0);
		v->play();
	}

}
void VideoRenderElement::Stop()
{
	video::IVideoSource* v = (video::IVideoSource*)m_video->GetGrabber().pointer();

	if (v)
	{
		v->stop();
	}

}
bool VideoRenderElement::IsDone()
{
	video::IVideoSource* v = (video::IVideoSource*)m_video->GetGrabber().pointer();

	if (v)
	{
		return v->isDone();
	}
	return true;

}
void VideoRenderElement::Render(const math::rectf& rc, const video::SColor& clr)
{
	m_video->Blit();
	//if (m_visible)
	{
		video::TextureUnit tex;
		tex.SetTexture(m_video->GetTexture());
		Engine::getInstance().getDevice()->useTexture(0, &tex);
		Engine::getInstance().getDevice()->draw2DImage(rc, clr);
	}
}
void VideoRenderElement::Update(float dt)
{
	video::IVideoSource* v = (video::IVideoSource*)m_video->GetGrabber().pointer();
}
float VideoRenderElement::GetLength()
{
	video::IVideoSource* v = (video::IVideoSource*)m_video->GetGrabber().pointer();
	return v->getDuration();
}

float VideoRenderElement::GetTime()
{
	video::IVideoSource* v = (video::IVideoSource*)m_video->GetGrabber().pointer();
	return v->getTime();
}



}
}

