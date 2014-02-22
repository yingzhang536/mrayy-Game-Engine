

/********************************************************************
	created:	2014/02/03
	created:	3:2:2014   10:28
	filename: 	C:\Development\mrayEngine\Projects\AugTel\GeomDepthState.h
	file path:	C:\Development\mrayEngine\Projects\AugTel
	file base:	GeomDepthState
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GeomDepthState__
#define __GeomDepthState__


#include "IRenderingState.h"
#include "IGUIRenderer.h"

namespace mray
{
	namespace TBee
	{
		class OpenNIHandler;
	}
namespace AugTel
{
class GeomDepthState :public TBee::IRenderingState
{
protected:
	TBee::OpenNIHandler* m_openNiHandler;

	bool m_mouseDown;
	math::vector2d m_mousePos;
	GCPtr<GUI::IGUIRenderer> m_guiRenderer;
public:
	GeomDepthState();
	virtual~GeomDepthState();
	virtual void InitState();
	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual video::IRenderTarget* Render(const math::rectf& rc, TBee::ETargetEye eye);
	virtual void Update(float dt);

	virtual void LoadFromXML(xml::XMLElement* e);
};

}
}

#endif
