

#include "stdafx.h"
#include "VideoclipRenderingState.h"

#include "Application.h"


namespace mray
{
namespace VT
{


VideoclipRenderingState::VideoclipRenderingState()
{
	m_video[0]=new video::VideoGrabberTexture();
	m_video[1]=new video::VideoGrabberTexture();
}
VideoclipRenderingState::~VideoclipRenderingState()
{
	delete m_video[0];
	delete m_video[1];
}

void VideoclipRenderingState::InitState(Application* app)
{
	IRenderingState::InitState(app);
	int c=m_stereo?2:1;
	for(int i=0;i<c;++i)
	{
		if(m_fileName[i]!="")
		{
			video::IVideoSourcePtr vdo=app->GetVideoManager()->CreateVideoFromFile(m_fileName[i],video::EVideo_RGB);
			video::ITexturePtr tex=app->getDevice()->createEmptyTexture2D(true);

			m_video[i]->Set(vdo,tex);
		}

	}
}

void VideoclipRenderingState::OnEvent(Event* e)
{
}
void VideoclipRenderingState::OnEnter(IRenderingState*prev)
{
	int c=m_stereo?2:1;
	for(int i=0;i<c;++i)
	{
		video::IVideoSource* v=(video::IVideoSource*)m_video[i]->GetGrabber().pointer();

		if(v)
		{
			v->seek(0);
			v->play();
		}
	}
}
void VideoclipRenderingState::OnExit()
{
	IRenderingState::OnExit();
	int c=m_stereo?2:1;
	for(int i=0;i<c;++i)
	{
		video::IVideoSource* v=(video::IVideoSource*)m_video[i]->GetGrabber().pointer();

		if(v)
		{
			v->stop();
		}
	}
}
video::IRenderTarget* VideoclipRenderingState::Render(bool left,const math::rectf& rc)
{
	int index=left?0:1;
	if(!m_stereo)
		index=0;
	video::IRenderTarget*rt= IRenderingState::Render(left,rc);
	video::IVideoDevice* dev=Engine::getInstance().getDevice();
	dev->setRenderTarget(rt);

	m_video[index]->Blit();

	video::TextureUnit tex;
	tex.SetTexture(m_video[index]->GetTexture());
	dev->useTexture(0,&tex);
	dev->draw2DImage(math::rectf(math::vector2d(0),rc.getSize()),1);

	dev->setRenderTarget(0);
	dev->useTexture(0,0);


	return rt;
}

void VideoclipRenderingState::Update(float dt)
{
}
void VideoclipRenderingState::LoadFromXML(xml::XMLElement* e)
{
	IRenderingState::LoadFromXML(e);
	xml::XMLAttribute* attr;
	attr=e->getAttribute("LeftVideo");
	if(attr)
		m_fileName[0]=attr->value;
	attr=e->getAttribute("RightVideo");
	if(attr)
		m_fileName[1]=attr->value;

}


}
}

