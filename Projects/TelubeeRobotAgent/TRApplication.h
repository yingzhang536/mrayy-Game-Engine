

/********************************************************************
	created:	2013/12/01
	created:	1:12:2013   23:23
	filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\TRApplication.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
	file base:	TRApplication
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __TRApplication__
#define __TRApplication__

#include "CMRayApplication.h"
#include "GUIBatchRenderer.h"
#include "ICameraVideoGrabber.h"
#include "VideoGrabberTexture.h"
#include "GstVideoProvider.h"
#include "ViewPort.h"
#include "IRobotController.h"
#include "OpenNIHandler.h"
#include "GeomDepthRect.h"
#include "IUDPClient.h"
#include "OpenNIManager.h"
#include "CameraProfile.h"
#include "GstStreamBin.h"
#include "GstPlayerBin.h"

namespace mray
{

class RobotCommunicator;
class GstVideoGrabberImpl;
class IRobotCommunicatorListener;
class IMessageSink;
class TRApplication :public CMRayApplication, public scene::IViewportListener
{
protected:

	enum class EController
	{
		XBox,
		Logicool
	};

	enum class EMessages
	{
		DepthData = 1,
		DepthSize = 2,
		IsStereo = 3,
		CameraConfig = 4,
		CalibrationDone = 5,
		ReportMessage = 6,
		IRSensorMessage = 7,
		BumpSensorMessage = 8,
		BatteryLevel = 9,
	};

	bool m_robotInited;

	EController m_controller;
	scene::ViewPort* m_viewPort;

	GCPtr<GUI::GUIBatchRenderer> m_guiRender;

	GCPtr<video::ICameraVideoGrabber> m_cameras[2];
	GCPtr<video::IVideoGrabber> m_combinedCameras;
	GCPtr<video::GstStreamBin> m_streamers;
	GCPtr<video::GstPlayerBin> m_players;
	//GCPtr<video::GstNetworkVideoStreamer> m_streamer;

	video::VideoGrabberTexture m_cameraTextures[3];
	video::VideoGrabberTexture* m_playerGrabber;

	video::ITexturePtr m_rtTexture;
	video::IRenderTargetPtr m_renderTarget;;



	RobotCommunicator* m_robotCommunicator;

	IRobotCommunicatorListener* m_communicatorListener;
	IMessageSink* m_msgSink;

	TBee::OpenNIHandler* m_openNi;

	TBee::GeomDepthRect m_depthRect;

	CameraProfileManager* m_cameraProfileManager;
	core::string m_cameraProfile;

	network::NetAddress m_remoteAddr;
	network::IUDPClient* m_commChannel;
	GCPtr<OpenNIManager> m_openNIMngr;
	bool m_isLocal;
	bool m_streamAudio;
	bool m_depthSend;
	bool m_isStarted;


	int m_videoPort;

	math::vector2di m_resolution;

	bool m_debugging;


	core::string m_ip;

	struct _CameraInfo
	{
		CameraInfo ifo;
		int w, h, fps;
	}m_cameraIfo[2];

	EStreamingQuality m_quality;
	bool m_enableStream;

	struct DebugData
	{
		DebugData()
		{
			userConnected = false;
		}
		bool userConnected;
		network::NetAddress userAddress;
		RobotStatus robotData;

		math::vector2d collision;
		bool debug;
	}m_debugData;

	void _InitResources();

	bool m_startVideo;
public:
	TRApplication();
	virtual~TRApplication();


	virtual void onEvent(Event* event);

	virtual void init(const OptionContainer &extraOptions);

	virtual void draw(scene::ViewPort* vp);
	virtual void WindowPostRender(video::RenderWindow* wnd);
	virtual void update(float dt);
	virtual void onDone();

	virtual void onRenderDone(scene::ViewPort*vp);

	void OnUserConnected(const network::NetAddress& address, int videoPort, int audioPort, bool rtcp);
	void OnRobotStatus(RobotCommunicator* sender, const RobotStatus& status);
	void OnCollisionData(RobotCommunicator* sender, float left, float right);
	void OnUserDisconnected(RobotCommunicator* sender, const network::NetAddress& address);
	void OnCalibrationDone(RobotCommunicator* sender);
	void OnReportMessage(RobotCommunicator* sender, int code, const core::string& msg);

	void OnMessage(network::NetAddress* addr, const core::string& msg, const core::string& value);

	CameraProfileManager* LoadCameraProfiles(const core::string& path);
	CameraProfileManager* GetCameraProfileManager(){ return m_cameraProfileManager; }
};

}


#endif
