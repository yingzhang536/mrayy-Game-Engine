

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


#include "IEyesRenderingBaseState.h"
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
class CameraRenderingState:public IEyesRenderingBaseState
{
protected:

	struct CameraInfo
	{
		CameraInfo() :id(0), camera(0), videoGrabber(0)
		{
		}
		int id;

		video::VideoGrabberTexture* videoGrabber;

		video::ICameraVideoGrabber* camera;
	};


	CameraInfo m_cameraSource[2];
	
	int m_VerticalShift;

	bool m_capturing;
	int m_clickCount;
	math::vector2d m_firstClick;
	math::vector2d m_BoxSize;
	GUI::IGUIRenderer* m_guiRenderer;

	virtual math::vector2d _GetEyeResolution(int i) ;
	virtual video::ITexturePtr GetEyeTexture(int i) ;
	virtual math::rectf GetEyeTexCoords(int i) { return math::rectf(0, 0, 1, 1); }
	void _RenderUI(const math::rectf& rc);
public:
	CameraRenderingState();
	virtual~CameraRenderingState();

	virtual void InitState(Application* app);
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
