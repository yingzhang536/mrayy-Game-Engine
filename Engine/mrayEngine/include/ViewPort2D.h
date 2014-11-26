
/********************************************************************
	created:	2009/03/22
	created:	22:3:2009   0:54
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\src\ViewPort2D.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\src
	file base:	ViewPort2D
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___ViewPort2D___
#define ___ViewPort2D___

#include "IRenderTarget.h"

namespace mray{
namespace GUI{

	class IGUIElement;

class MRAY_DLL ViewPort2D
{
private:
protected:
	IGUIElement* m_elem;
	GCPtr<video::IRenderTarget>m_rt;
	video::SColor m_clearColor;
public:
	ViewPort2D(IGUIElement* elem,const video::IRenderTargetPtr& rt);
	virtual~ViewPort2D();

	virtual void draw();

	IGUIElement* getElement();
	const video::IRenderTargetPtr& getRenderTarget();

	void setElement(IGUIElement*  elem);
	void setRenderTarget(const video::IRenderTargetPtr& rt);
};

}
}


#endif //___ViewPort2D___
