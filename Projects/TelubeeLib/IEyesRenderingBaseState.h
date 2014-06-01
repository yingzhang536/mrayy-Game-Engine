

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
	class ICameraVideoSource;

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

	float m_targetAspectRatio;
	float m_hmdDistance;
	float m_cameraFov;
	float m_hmdFov;
	math::vector2d m_hmdSize;
	float m_panningScale;//used to give some freedom to look around using Oculus in a scaled up mode
	float m_headPan;
	float m_headTilt;
	bool m_enablePanning;

	bool m_useLensCorrection;
	math::vector4d m_correctionParamsU;
	math::vector4d m_correctionParamsV;

	GUI::IGUIRenderer* m_guiRenderer;

	GCPtr<video::ParsedShaderPP> m_lensCorrectionPP;
	video::ParsedShaderPP::PassValue* m_correctionValue[2];
	CRobotConnector* m_robotConnector;
	ICameraVideoSource* m_videoSource;

	void _UpdateCameraParams();

	virtual void _RenderUI(const math::rectf& rc,math::vector2d& pos);

public:
	IEyesRenderingBaseState(const core::string& name);
	virtual~IEyesRenderingBaseState();

	virtual void InitState();
	void SetParameters(float targetAspectRatio, float hmdDistance, float cameraFov, float hmdFov);

	void SetVideoSource(ICameraVideoSource* src){ m_videoSource = src; }
	ICameraVideoSource* GetVideoSource(){ return m_videoSource; }

	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual void Update(float dt);
	virtual video::IRenderTarget* Render(const math::rectf& rc, ETargetEye eye);

	virtual void LoadFromXML(xml::XMLElement* e);
	


};

}
}


#endif
