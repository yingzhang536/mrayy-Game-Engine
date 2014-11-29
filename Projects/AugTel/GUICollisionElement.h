

#ifndef GUICollisionElement_h__
#define GUICollisionElement_h__

#include "GUICollisionLayout.h"

namespace mray
{
namespace GUI
{

class GUICollisionElement :public IGUIPanelElement, public GUICollisionLayout
{
	DECLARE_RTTI;
	static const GUID ElementType;
protected:

	IGUIElement* m_indicators[6];
	float m_bumpers[6];
public:
	GUICollisionElement(IGUIManager* m);
	virtual ~GUICollisionElement();

	void SetSensors(int count, float *v);
	void SetBumper(int count, bool *v);

	virtual void Update(float dt);

	virtual void DoneLoadingElements();
};
DECLARE_ELEMENT_FACTORY(GUICollisionElement);

}
}

#endif // GUICollisionElement_h__
