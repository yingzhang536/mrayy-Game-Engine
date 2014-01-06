

/********************************************************************
created:	2013/12/04
created:	4:12:2013   11:27
filename: 	C:\Development\mrayEngine\Projects\TELUBee\IEyesRenderingBaseState.h
file path:	C:\Development\mrayEngine\Projects\TELUBee
file base:	IEyesRenderingBaseState
file ext:	h
author:		MHD Yamen Saraiji

purpose:
*********************************************************************/

#ifndef __IEyesRenderingBaseState__
#define __IEyesRenderingBaseState__

#include "IRenderingState.h"
#include "ParsedShaderPP.h"



namespace mray
{

namespace TBee
{
	class IRobotCommunicator;
	class CRobotConnector;

class IEyesRenderingBaseState :public IRenderingState
{
protected:

	struct EyeInfo
	{
		EyeInfo() :ratio(1), flip90(false), cw(false)
		{
		}
		bool flip90;
		bool cw;

		math::vector2d cropping;
		math::vector2d scale;
		float ratio;
	};
	EyeInfo m_eyes[2];
	math::vector2d m_cameraResolution;
	int m_cameraFPS;

	float m_targetAspectRatio;
	float m_hmdDistance;
	float m_cameraFov;
	float m_hmdFov;
	math::vector2d m_hmdSize;

	GUI::IGUIRenderer* m_guiRenderer;

	GCPtr<video::ParsedShaderPP> m_lensCorrectionPP;
	video::ParsedShaderPP::PassValue* m_correctionValue[2];
	CRobotConnector* m_robotConnector;

	void _UpdateCameraParams();

	virtual math::vector2d _GetEyeScalingFactor(int i) { return math::vector2d(1, 1); }
	virtual math::vector2d _GetEyeResolution(int i) = 0;
	virtual video::ITexturePtr GetEyeTexture(int i) = 0;
	virtual math::rectf GetEyeTexCoords(int i) = 0;
	virtual void _RenderUI(const math::rectf& rc);

public:
	IEyesRenderingBaseState();
	virtual~IEyesRenderingBaseState();

	virtual void InitState(Application* app);
	void SetParameters(float targetAspectRatio, float hmdDistance, float cameraFov, float hmdFov);

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
