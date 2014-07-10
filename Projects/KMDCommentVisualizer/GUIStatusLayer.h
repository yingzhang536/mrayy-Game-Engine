
#ifndef GUIStatusLayer_h__
#define GUIStatusLayer_h__


#include "GUIStatusLayerLayout.h"
#include "GUIDockedElement.h"
#include "SessionContainer.h"

#include <queue>

namespace mray
{
namespace GUI
{
class GUICommentDetailsPanel;

class GUIStatusLayer :public IGUIPanelElement, public GUIStatusLayerLayout, public GUIDockedElement
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:

public:
	GUIStatusLayer(GUI::IGUIManager* m);
	virtual ~GUIStatusLayer();

	void SetProjects(kmd::SessionContainer* sessions);

	virtual void Draw(const math::rectf*vp);
	virtual void Update(float dt);
};
DECLARE_ELEMENT_FACTORY(GUIStatusLayer);

}
}


#endif // GUIStatusLayer_h__
