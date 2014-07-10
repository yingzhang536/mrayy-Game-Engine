

#ifndef GenericRenderLayer_h__
#define GenericRenderLayer_h__

#include "IRenderingLayer.h"

namespace mray
{
namespace kmd
{
	
class GenericRenderLayer:public IRenderingLayer
{
protected:
	GUI::IGUIPanelElement* m_guiPanel;
public:
	GenericRenderLayer();
	virtual ~GenericRenderLayer();

	virtual void InitLayer(GUI::IGUIPanelElement* panel);
	virtual void UpdateLayer(float dt) ;
	GUI::IGUIPanelElement* GetPanelElement(){ return m_guiPanel; }

	virtual void SetVisible(bool v)
	{
		if (m_guiPanel)
			m_guiPanel->SetVisible(v);
	}
	virtual bool IsVisible()
	{

		if (m_guiPanel)
			return m_guiPanel->IsVisible();
		return false;
	}
};

}
}

#endif // GenericRenderLayer_h__
