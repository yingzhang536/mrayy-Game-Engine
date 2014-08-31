

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
#include "ParsedShaderPP.h"
#include "ARServiceProvider.h"
#include "GUIManager.h"
#include "GUIConsole.h"

namespace mray
{
	namespace TBee
	{
		class CRobotConnector;
		class ICameraVideoSource;
		class CalibHeadController;
		class TelubeeCameraConfiguration;
	}
namespace NCam
{
	class NissanRobotCommunicator;

	class RobotCameraState :public TBee::IRenderingState, public scene::IViewportListener,public IARServiceListener
{
protected:

	struct SurfaceMeshParams
	{
		bool plane;
		float hfov,vfov;
		int segments;
		float radius;
		math::vector3d scale[2];
	};
	scene::MeshRenderableNode* m_surface[2];
	SurfaceMeshParams m_surfaceParams;

	typedef IRenderingState Parent;

	GCPtr<GUI::GUIManager> m_guimngr;

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
	NissanRobotCommunicator* m_robotComm;

	ARServiceProvider* m_arServiceProvider;

	GUI::GUIConsole* m_console;

	float m_hmdFov;

	bool m_useLensCorrection;
	GCPtr<video::ParsedShaderPP> m_lensCorrectionPP;
	TBee::TelubeeCameraConfiguration *m_cameraConfiguration;
	bool m_camConfigDirty;

	void GenerateSurface(bool plane,float hfov, float vfov, int segments,  float cameraScreenDistance);
	void _RenderUI(const math::rectf& rc);
	void RescaleMesh(int index,const math::vector3d &scaleFactor);
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

	virtual void OnARContents(ARCommandAddData* cmd);
	virtual void OnVechicleData();
	virtual void OnDeletedGroup(ARCommandDeleteGroup* cmd);
};

}
}


#endif
