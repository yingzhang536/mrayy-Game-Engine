

#ifndef GUILayersContainer_h__
#define GUILayersContainer_h__

#include "IGUIPanelElement.h"
#include "GUIDockedElement.h"
#include "GUIElementFactory.h"


namespace mray
{
namespace GUI
{
	
class GUILayersContainer :public IGUIPanelElement, public GUIDockedElement
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
public:

	GUILayersContainer(GUI::IGUIManager* m);
	virtual ~GUILayersContainer();

	virtual void Draw(const math::rectf*vp);
	
};
DECLARE_ELEMENT_FACTORY(GUILayersContainer);

}
}

#endif // GUILayersContainer_h__
