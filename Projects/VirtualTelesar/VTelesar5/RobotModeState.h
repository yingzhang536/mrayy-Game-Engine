

/********************************************************************
	created:	2013/02/19
	created:	19:2:2013   21:53
	filename: 	C:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\RobotModeState.h
	file path:	C:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	RobotModeState
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __RobotModeState__
#define __RobotModeState__

#include "IRenderingState.h"
#include "VideoGrabberTexture.h"

namespace mray
{
namespace VT
{


class RobotModeState:public IRenderingState
{
protected:

	core::string m_fileName;
	video::VideoGrabberTexture* m_video;

public:
	RobotModeState();
	virtual~RobotModeState();

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
