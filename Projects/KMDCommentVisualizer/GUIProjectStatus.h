

#ifndef GUIProjectStatus_h__
#define GUIProjectStatus_h__


#include "GUIProjectStatusLayout.h"
#include "SessionDetails.h"
#include "GUIDockedElement.h"

namespace mray
{
namespace GUI
{

class GUIProjectStatus :public IGUIPanelElement, public GUIProjectStatusLayout
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:

	kmd::SessionDetails* m_project;
public:
	GUIProjectStatus(IGUIManager* m);
	virtual ~GUIProjectStatus();

	void SetProject(kmd::SessionDetails* p);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);

};
DECLARE_ELEMENT_FACTORY(GUIProjectStatus);


}
}

#endif // GUIProjectStatus_h__
