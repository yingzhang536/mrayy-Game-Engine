

/********************************************************************
	created:	2012/10/09
	created:	9:10:2012   22:41
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\VideoclipRenderingState.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	VideoclipRenderingState
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___VideoclipRenderingState___
#define ___VideoclipRenderingState___


#include "IRenderingState.h"
#include "VideoGrabberTexture.h"
#include "IRenderTarget.h"

namespace mray
{
namespace VT
{

class VideoclipRenderingState:public IRenderingState
{
protected:

	core::string m_fileName[2];
	video::VideoGrabberTexture* m_video[2];

public:
	VideoclipRenderingState();
	virtual~VideoclipRenderingState();
	
	void SetFileName(bool left,const core::string&name){m_fileName[left?0:1]=name;}

	virtual void InitState(Application* app);

	virtual void OnEvent(Event* e);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual void Update(float dt);
	virtual video::IRenderTarget* Render(bool left,const math::rectf& rc);

	virtual void LoadFromXML(xml::XMLElement* e);

};

}
}

#endif
