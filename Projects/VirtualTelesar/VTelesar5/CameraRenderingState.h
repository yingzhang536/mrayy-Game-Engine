

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
#include "CameraTextureSource.h"
#include "IRenderTarget.h"



namespace mray
{
namespace VT
{

class CameraRenderingState:public IRenderingState
{
protected:

	struct CameraInfo
	{
		CameraInfo():id(0),width(320),height(240),fps(25)
		{
		}
		int id;
		int width;
		int height;
		int fps;
	};

	video::CameraTextureSource* m_cams[2];

	int m_VerticalShift;

	float m_texCoordRectSize;

public:
	CameraRenderingState();
	virtual~CameraRenderingState();

	virtual void InitState(Application* app);
	video::CameraTextureSource* GetLeftCamera(){return m_cams[0];}
	video::CameraTextureSource* GetRightCamera(){return m_cams[1];}

	virtual void OnEvent(Event* e);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual void Update(float dt);
	virtual video::IRenderTarget* Render(bool left,const math::rectf& rc);

	virtual void LoadFromXML(xml::XMLElement* e);
	virtual bool CanSleep(){return false;}

};

}
}

#endif
