

/********************************************************************
	created:	2012/10/10
	created:	10:10:2012   18:39
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\CameraRenderingState.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	CameraRenderingState
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___CameraRenderingState___
#define ___CameraRenderingState___


#include "IRenderingState.h"
#include "VideoGrabberTexture.h"
#include "IRenderTarget.h"
#include "ParsedShaderPP.h"


namespace mray
{
	namespace video
	{
		class ICameraVideoGrabber;
	}
namespace TBee
{
	class IRobotCommunicator;
	class CRobotConnector;
class CameraRenderingState:public IRenderingState
{
protected:

	struct CameraInfo
	{
		CameraInfo():id(0),camera(0),videoGrabber(0)
		{
		}
		int id;

		math::vector2d cropping;
		math::vector2d scale;
		float ratio;

		video::VideoGrabberTexture* videoGrabber;

		video::ICameraVideoGrabber* camera;
	};

	math::vector2d m_cameraResolution;
	int m_cameraFPS;

	float m_targetAspectRatio;
	float m_hmdDistance;
	float m_cameraFov;
	float m_hmdFov;
	math::vector2d m_hmdSize;

	CameraInfo m_cameraSource[2];
	
	int m_VerticalShift;

	bool m_capturing;
	int m_clickCount;
	math::vector2d m_firstClick;
	math::vector2d m_BoxSize;

	GCPtr<video::ParsedShaderPP> m_lensCorrectionPP;
	video::ParsedShaderPP::PassValue* m_correctionValue[2];


	CRobotConnector* m_robotConnector;

	GUI::IGUIRenderer* m_guiRenderer;

	void _RenderUI(const math::rectf& rc);
	void _UpdateCameraParams();
public:
	CameraRenderingState();
	virtual~CameraRenderingState();

	virtual void InitState(Application* app);
	void SetParameters(float targetAspectRatio,float hmdDistance,float cameraFov,float hmdFov);
	void SetCameraInfo(ETargetEye eye,int id);

	video::ICameraVideoGrabber* GetCamera(ETargetEye eye){return m_cameraSource[GetEyeIndex(eye)].camera;}


	virtual void OnEvent(Event* e);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual void Update(float dt);
	virtual video::IRenderTarget* Render(const math::rectf& rc,ETargetEye eye);

	virtual void LoadFromXML(xml::XMLElement* e);

};

}
}

#endif
