

#include "stdafx.h"
#include "OpenNICaptureDevice.h"
#include "ILogManager.h"


namespace mray
{

OpenNICaptureDevice::OpenNICaptureDevice(const char* deviceURI):m_uri(deviceURI)
{
	m_isOpen=false;
	m_pUserTracker = 0;
	Open();
}
OpenNICaptureDevice::~OpenNICaptureDevice()
{
	Close();
}


openni::Status OpenNICaptureDevice::Open()
{
	Close();
	openni::Status rc = m_device.open(m_uri);
	if (rc != openni::STATUS_OK)
	{
		gLogManager.log("Failed to open device:"+ core::string(openni::OpenNI::getExtendedError()),ELL_WARNING);
		return rc;
	}
	rc=m_depthStream.create(m_device,openni::SENSOR_DEPTH);
	if(rc==openni::STATUS_OK)
	{
		openni::VideoMode mode=m_depthStream.getVideoMode();
		mode.setResolution(320,240);
		m_depthStream.setVideoMode(mode);
		rc=m_depthStream.start();
		if(rc!=openni::STATUS_OK)
		{
			gLogManager.log("Failed to start depth sensor: "+core::string(m_device.getDeviceInfo().getName()),ELL_WARNING);
			m_depthStream.destroy();
			m_device.close();
			return rc;
		}
	}else
	{
		gLogManager.log("Failed to find depth sensor: "+core::string(m_device.getDeviceInfo().getName()),ELL_WARNING);
		m_device.close();
		return rc;
	}
	rc=m_colorStream.create(m_device,openni::SENSOR_COLOR);
	if(rc==openni::STATUS_OK)
	{
		openni::VideoMode mode=m_colorStream.getVideoMode();
		mode.setResolution(320,240);
		m_colorStream.setVideoMode(mode);
		rc=m_colorStream.start();
		if(rc!=openni::STATUS_OK)
		{
			gLogManager.log("Failed to start color sensor: "+core::string(m_device.getDeviceInfo().getName()),ELL_WARNING);
			m_depthStream.destroy();
			m_colorStream.destroy();
			m_device.close();
			return rc;
		}
	}else
	{
		gLogManager.log("Failed to find color sensor: "+core::string(m_device.getDeviceInfo().getName()),ELL_WARNING);
		m_depthStream.destroy();
		m_device.close();
		return rc;
	}

	m_pUserTracker = new nite::UserTracker;
	if (m_pUserTracker->create(&m_device) != nite::STATUS_OK)
	{
		m_depthStream.destroy();
		m_colorStream.destroy();
		m_device.close();
		delete m_pUserTracker;
		return openni::STATUS_ERROR;
	}
	m_isOpen=true;
	return openni::STATUS_OK;
}
bool OpenNICaptureDevice::IsOpen()
{
	return m_isOpen;
}
void OpenNICaptureDevice::Close()
{
	if(!m_isOpen)
		return;
	m_isOpen=false;
	m_depthStream.destroy();
	m_colorStream.destroy();
	m_userTrackerFrame.release();
	delete m_pUserTracker;
	m_pUserTracker=0;
	m_device.close();
}

bool OpenNICaptureDevice::Update()
{
	if(!m_isOpen)
		return false;
	nite::Status rc = m_pUserTracker->readFrame(&m_userTrackerFrame);
	if (rc != nite::STATUS_OK)
	{
		gLogManager.log("GetNextData failed",ELL_WARNING,EVL_Heavy);
		return false;
	}
	return true;
}



}



