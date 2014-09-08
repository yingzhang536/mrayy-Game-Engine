
#include "stdafx.h"
#include "LocalCameraVideoSource.h"
#include "ICameraVideoGrabber.h"

#include "DirectShowVideoGrabber.h"


namespace mray
{
namespace TBee
{

	typedef video::DirectShowVideoGrabber VCameraType;
	LocalCameraVideoSource::LocalCameraVideoSource(int c1 , int c2)
{
	m_cameraResolution.set(640, 480);
	m_cameraFPS = 25;
	
	m_cameraSource[0].id = c1;
	m_cameraSource[1].id = c2;

	m_cameraSource[0].videoGrabber = new video::VideoGrabberTexture();
	m_cameraSource[1].videoGrabber = new video::VideoGrabberTexture();

	m_started = false;
}
LocalCameraVideoSource::~LocalCameraVideoSource()
{

	delete m_cameraSource[0].videoGrabber;
	delete m_cameraSource[1].videoGrabber;
}

void LocalCameraVideoSource::SetCameraResolution(const math::vector2d& res, int fps)
{
	m_cameraResolution = res;
	m_cameraFPS = fps;

	if (m_started)
	{
		for (int i = 0; i < 2;++i)
		{
			if (m_cameraSource[i].camera)
			{
				m_cameraSource[i].camera->Stop();
				m_cameraSource[i].camera->InitDevice(m_cameraSource[i].id, m_cameraResolution.x, m_cameraResolution.y, m_cameraFPS);
				m_cameraSource[i].camera->Start();

			}
		}
	}
}
void LocalCameraVideoSource::Init()
{
	for (int i = 0; i < 2; ++i)
	{
		//m_eyes[i].flip90 = true;
		m_cameraSource[i].camera = new VCameraType();
		video::ITexturePtr tex = Engine::getInstance().getDevice()->createEmptyTexture2D(true);

		m_cameraSource[i].videoGrabber->Set(m_cameraSource[i].camera, tex);
	}
}
void LocalCameraVideoSource::Open()
{
	m_started = true;
	m_cameraSource[0].camera->InitDevice(m_cameraSource[0].id, m_cameraResolution.x, m_cameraResolution.y, m_cameraFPS);
	if (m_cameraSource[1].camera)
		m_cameraSource[1].camera->InitDevice(m_cameraSource[1].id, m_cameraResolution.x, m_cameraResolution.y, m_cameraFPS);


	for (int i = 0; i < 2; ++i)
	{
		if (m_cameraSource[i].camera)
		{
			m_cameraSource[i].camera->SetParameter(video::ICameraVideoGrabber::Param_Focus, "0.5");
			core::string val = m_cameraSource[i].camera->GetParameter(video::ICameraVideoGrabber::Param_Focus);
			printf("focus value is: %s\n", val.c_str());
		}
	}

}
void LocalCameraVideoSource::Close()
{
	m_started = false;
	m_cameraSource[0].camera->Stop();
	if (m_cameraSource[1].camera)
		m_cameraSource[1].camera->Stop();
}
void LocalCameraVideoSource::SetCameraID(int i, int cam)
{
	m_cameraSource[i].id = cam;
}
math::vector2d LocalCameraVideoSource::GetEyeResolution(int i)
{
	return m_cameraSource[i].camera->GetFrameSize();
}
video::ITexturePtr LocalCameraVideoSource::GetEyeTexture(int i)
{
	return m_cameraSource[i].videoGrabber->GetTexture();
}

bool LocalCameraVideoSource::Blit()
{
	bool a=m_cameraSource[0].videoGrabber->Blit();
	bool b=m_cameraSource[1].videoGrabber->Blit();
	return a || b;
}
void LocalCameraVideoSource::LoadFromXML(xml::XMLElement* e)
{

	xml::XMLAttribute* attr;

	attr = e->getAttribute("CameraLeft");
	if (attr)
		m_cameraSource[0].id = core::StringConverter::toInt(attr->value);
	attr = e->getAttribute("CameraRight");
	if (attr)
		m_cameraSource[1].id = core::StringConverter::toInt(attr->value);


	attr = e->getAttribute("Size");
	if (attr)
	{
		m_cameraResolution = core::StringConverter::toVector2d(attr->value);
	}
	attr = e->getAttribute("FPS");
	if (attr)
	{
		m_cameraFPS = core::StringConverter::toInt(attr->value);
	}

}

}
}
