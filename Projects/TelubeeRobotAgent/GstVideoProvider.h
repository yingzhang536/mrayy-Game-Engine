

/********************************************************************
	created:	2013/12/01
	created:	1:12:2013   23:10
	filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\GstVideoProvider.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
	file base:	GstVideoProvider
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GstVideoProvider__
#define __GstVideoProvider__

#include "IVideoGrabber.h"
#include "NetAddress.h"

namespace mray
{

	class GstVideoProviderImpl;

class GstVideoProvider
{
protected:
	GstVideoProviderImpl* m_impl;
public:
	GstVideoProvider();
	virtual~GstVideoProvider();

	void SetDataSource(video::IVideoGrabber* src);
	void StreamDataTo(const network::NetAddress& target);

	void Start();
	void Stop();
};

}


#endif
