

/********************************************************************
	created:	2014/02/17
	created:	17:2:2014   4:16
	filename: 	C:\Development\mrayEngine\Projects\AugTel\DataCommunicator.h
	file path:	C:\Development\mrayEngine\Projects\AugTel
	file base:	DataCommunicator
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __DataCommunicator__
#define __DataCommunicator__





#include "IUDPClient.h"

namespace mray
{
	namespace TBee
	{
		class GeomDepthRect;
	}
namespace AugTel
{

	class IDataCommunicatorListener
	{
	public:
		virtual void OnDepthData(const TBee::GeomDepthRect& dpRect){}
		virtual void OnDepthSize(const math::vector2di &sz){}
		virtual void OnIsStereoImages(bool isStereo){}
		virtual void OnCameraConfig(const core::string& cameraProfile){}
		virtual void OnRobotCalibrationDone(){}
		virtual void OnReportedMessage(int code,const core::string& msg){}
	};

class DataCommunicator:public ListenerContainer<IDataCommunicatorListener*>
{
protected:

	network::IUDPClient* m_client;
	OS::IThread* m_thread;

	DECLARE_FIRE_METHOD(OnDepthData, (const TBee::GeomDepthRect& dpRect), (dpRect));
	DECLARE_FIRE_METHOD(OnDepthSize, (const math::vector2di &sz), (sz));
	DECLARE_FIRE_METHOD(OnIsStereoImages, (bool isStereo), (isStereo));
	DECLARE_FIRE_METHOD(OnCameraConfig, (const core::string& cameraProfile), (cameraProfile));
	DECLARE_FIRE_METHOD(OnRobotCalibrationDone, (), ());
	DECLARE_FIRE_METHOD(OnReportedMessage, (int code, const core::string& msg), (code,msg));
public:
	DataCommunicator();
	virtual~DataCommunicator();

	void Start(int port);
	void Stop();

	int _Process();
};

}
}


#endif
