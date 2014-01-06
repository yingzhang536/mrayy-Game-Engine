

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
#include "TelubeeRobotDLL.h"

namespace mray
{

class RobotCommunicator;
class GstVideoGrabberImpl;
class IRobotCommunicatorListener;
class TRApplication :public CMRayApplication, public scene::IViewportListener
{
protected:

	enum class EController
	{
		XBox,
		Logicool
	};

	EController m_controller;
	scene::ViewPort* m_viewPort;

	GCPtr<GUI::GUIBatchRenderer> m_guiRender;

	GCPtr<video::ICameraVideoGrabber> m_cameras[2];
	GCPtr<video::IVideoGrabber> m_combinedCameras;

	video::VideoGrabberTexture m_cameraTextures[3];

	video::ITexturePtr m_rtTexture;
	video::IRenderTargetPtr m_renderTarget;;


	GstVideoProvider* m_videoProvider;
	GstVideoGrabberImpl* m_videoGrabber;

	RobotCommunicator* m_robotCommunicator;

	IRobotCommunicatorListener* m_communicatorListener;

	bool m_isLocal;
	bool m_streamAudio;

	core::string m_ip;

	struct CameraInfo
	{
		int id;
		int w, h, fps;
	}m_cameraIfo[2];

	EStreamingQuality m_quality;

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

	void OnUserConnected(const network::NetAddress& address,int videoPort,int audioPort);
	void OnRobotStatus(RobotCommunicator* sender, const RobotStatus& status);
	void OnCollisionData(RobotCommunicator* sender, float left, float right);
	void OnUserDisconnected(RobotCommunicator* sender, const network::NetAddress& address);

};

}


#endif
