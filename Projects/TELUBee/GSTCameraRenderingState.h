

/********************************************************************
created:	2012/10/10
created:	10:10:2012   18:39
filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\GSTCameraRenderingState.h
file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
file base:	GSTCameraRenderingState
file ext:	h
author:		MHD YAMEN SARAIJI

purpose:
*********************************************************************/
#ifndef ___GSTCameraRenderingState___
#define ___GSTCameraRenderingState___


#include "IEyesRenderingBaseState.h"
#include "GStreamVideoProvider.h"


namespace mray
{
namespace video
{
class ICameraVideoGrabber;
}
namespace TBee
{
class GSTCameraRenderingState :public IEyesRenderingBaseState
{
protected:
	GStreamVideoProvider* m_providers;

	video::ITexturePtr m_remoteTex;

	std::string m_hostIp;
	int m_hostPort;
	int m_localPort;

	int m_frameCounter;
	int m_fps;
	float m_timeAcc;

	virtual math::vector2d _GetEyeScalingFactor(int i){ return math::vector2d(1, 1); }
	virtual math::vector2d _GetEyeResolution(int i);
	virtual video::ITexturePtr GetEyeTexture(int i);
	virtual math::rectf GetEyeTexCoords(int i) { return math::rectf(i*0.5, 0, 0.5+i*0.5, 1); }

	virtual void _RenderUI(const math::rectf& rc);
public:
	GSTCameraRenderingState();
	virtual~GSTCameraRenderingState();

	virtual void InitState(Application* app);

	virtual void OnEvent(Event* e);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual void Update(float dt);
	virtual video::IRenderTarget* Render(const math::rectf& rc, ETargetEye eye);

	virtual void LoadFromXML(xml::XMLElement* e);

};

}
}

#endif
