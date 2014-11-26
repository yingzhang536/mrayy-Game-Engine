

#ifndef GUINavigationElement_h__
#define GUINavigationElement_h__

#include "GUINavigationLayout.h"

namespace mray
{
namespace GUI
{
	
class GUINavigationElement :public IGUIPanelElement,public GUINavigationLayout
{
	DECLARE_RTTI;
	static const GUID ElementType;
protected:
	struct CValues
	{
		math::vector2d speed;
		float rotation;
		math::vector2d head;

	};

	CValues m_current;
	CValues m_target;

	float m_leftPos;
	float m_rightPos;
	float m_frontPos;
	float m_backPos;

public:
	GUINavigationElement(IGUIManager* m);
	virtual ~GUINavigationElement();

	void SetSpeed(float x, float y,float rotation,float pan,float tilt);

	virtual void Update(float dt);

	virtual void DoneLoadingElements();
};
DECLARE_ELEMENT_FACTORY(GUINavigationElement);

}
}

#endif // GUINavigationElement_h__
