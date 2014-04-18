

/********************************************************************
	created:	2014/03/05
	created:	5:3:2014   1:48
	filename: 	C:\Development\mrayEngine\Projects\NissanCamera\RobotCameraState.h
	file path:	C:\Development\mrayEngine\Projects\NissanCamera
	file base:	RobotCameraState
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __RobotCameraState__
#define __RobotCameraState__

#include "IRenderingState.h"
#include "SceneManager.h"
#include "ViewPort.h"

namespace mray
{
	namespace TBee
	{
		class CRobotConnector;
		class ICameraVideoSource;
		class CalibHeadController;
	}
namespace NCam
{
	class RobotCameraState :public TBee::IRenderingState, public scene::IViewportListener
{
protected:
	typedef IRenderingState Parent;

	GCPtr<scene::SceneManager> m_sceneManager;
	GCPtr<scene::ViewPort> m_viewport[2];
	scene::CameraNode* m_camera[2];
	scene::ISceneNode* m_headNode;
	scene::ISceneNode* m_screenNode[2];
	video::RenderPass* m_screenMtrl[2];
	TBee::CRobotConnector* m_robotConnector;
	TBee::ICameraVideoSource* m_videoSource;
	GUI::IGUIRenderer* m_guiRenderer;
	bool m_lockAxis[3];

	TBee::CalibHeadController* m_headController;

	float m_hmdFov;


	void _RenderUI(const math::rectf& rc);
public:
	RobotCameraState();
	virtual~RobotCameraState();


	void SetCameraInfo(TBee::ETargetEye eye, int id);

	virtual void InitState();

	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void OnEnter(TBee::IRenderingState*prev);
	virtual void OnExit();
	virtual void Update(float dt);
	virtual video::IRenderTarget* Render(const math::rectf& rc, TBee::ETargetEye eye);

	virtual void LoadFromXML(xml::XMLElement* e);
	virtual xml::XMLElement* WriteToXML(xml::XMLElement* e);

};

}
}


#endif
