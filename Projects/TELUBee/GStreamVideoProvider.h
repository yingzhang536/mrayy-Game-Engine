

/********************************************************************
	created:	2013/03/12
	created:	12:3:2013   21:27
	filename: 	D:\Development\mrayEngine\Projects\TELUBee\GStreamVideoProvider.h
	file path:	D:\Development\mrayEngine\Projects\TELUBee
	file base:	GStreamVideoProvider
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/
#ifndef _GStreamVideoProvider_
#define _GStreamVideoProvider_

#include "IVideoProvider.h"
class ofGstVideoPlayer;
namespace mray
{
namespace TBee
{

class GStreamVideoProvider:public IVideoProvider
{
protected:
	ofGstVideoPlayer* m_playBack;
public:
	GStreamVideoProvider();
	virtual~GStreamVideoProvider();

	void LoadMovie(const core::string& path);
	virtual void ConnectToCameras(const core::string& ip, int videoPort,int audioPort, int localPort);
	virtual void Disconnect();
	virtual bool IsConnected();

	virtual bool HasNewImage(int id);
	virtual const video::ImageInfo* GetImage(int id);
	math::rectf	GetTexRect(int id);

	virtual void Update(float dt);
};


}
}

#endif
