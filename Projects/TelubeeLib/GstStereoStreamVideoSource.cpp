

#include "stdafx.h"
#include "GstStereoStreamVideoSource.h"
#include "GstSingleNetVideoSource.h"




namespace mray
{
namespace TBee
{

GstStereoStreamVideoSource::GstStereoStreamVideoSource(const core::string& ip1 , int port1 , const core::string& ip2 , int port2 )
{
	m_sources[0] = new GstSingleNetVideoSource(ip1, port1);
	m_sources[1] = new GstSingleNetVideoSource(ip2, port2);
}
GstStereoStreamVideoSource::~GstStereoStreamVideoSource()
{
	delete m_sources[0];
	delete m_sources[1];
}

void GstStereoStreamVideoSource::SetIPs(const core::string& ip1, const core::string& ip2)
{
	m_sources[0]->SetIP(ip1);
	m_sources[1]->SetIP(ip2);
}
void GstStereoStreamVideoSource::SetPorts(int port1, int port2)
{
	m_sources[0]->SetPort(port1);
	m_sources[1]->SetPort(port2);
}

void GstStereoStreamVideoSource::Init()
{
	m_sources[0]->Init();
	m_sources[1]->Init();
}
void GstStereoStreamVideoSource::Open()
{
	m_sources[0]->Open();
	m_sources[1]->Open();
}
void GstStereoStreamVideoSource::Close()
{
	m_sources[0]->Close();
	m_sources[1]->Close();
}
bool GstStereoStreamVideoSource::Blit()
{
	bool ret = false;
	ret |= m_sources[0]->Blit();
	ret |= m_sources[1]->Blit();
	return ret;
}

math::vector2d GstStereoStreamVideoSource::GetEyeResolution(int i)
{
	return m_sources[i]->GetEyeResolution(0);
}
video::ITexturePtr GstStereoStreamVideoSource::GetEyeTexture(int i)
{
	return m_sources[i]->GetEyeTexture(0);
}

}
}

