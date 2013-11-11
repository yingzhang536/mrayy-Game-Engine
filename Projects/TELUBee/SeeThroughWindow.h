


/********************************************************************
	created:	2013/02/03
	created:	3:2:2013   12:46
	filename: 	C:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\SeeThroughWindow.h
	file path:	C:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	SeeThroughWindow
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __SeeThroughWindow__
#define __SeeThroughWindow__


#include "Viewport.h"
#include "IRenderingState.h"
#include "IEventReciver.h"


namespace mray
{
	namespace GUI
	{
		class IGUIManager;
	}
namespace TBee
{
	class Application;
	class HMDCameraRenderer;
	class HMDRobotRenderer;

class SeeThroughWindow:public scene::IViewportListener,public IEventReciver
{
protected:

	struct CameraParams
	{
		core::string name;
		core::string value;
	};
	std::vector<CameraParams> m_cameraParams;

	HMDCameraRenderer* m_camera;
	HMDRobotRenderer*m_robot;
	
	video::RenderWindow* m_2ndWnd;
	scene::ViewPort* m_vp;
	scene::ViewPort* m_eyes[2];

	video::ITexturePtr m_rtTex[2];
	video::IRenderTargetPtr m_rt[2];

	GUI::IGUIManager* m_guiManager;

	GCPtr<InputManager> m_inputManager;

	enum EMode
	{
		None,
		SeeThrough,
		RemoteVision
	};
	EMode m_mode;

	float m_lookAtScreenFactor;

	void _SetCamera(ETargetEye eye,int id);
public:
	SeeThroughWindow();
	virtual~SeeThroughWindow();

	void Init(Application* app,const OptionContainer& appOptions);
	virtual void onRenderDone(scene::ViewPort*vp);

	virtual void onEvent(Event* e);

	void ConnectToCameras(const core::string& ip,int port1,int port2);
	void Disconnect();
	bool IsConnected();
	void SwitchToSeeThrough();
	void SwitchToRemoteVision();

	void Update(float dt);

	void LoadFromXML(xml::XMLElement* e);
};

}
}

#endif