

#ifndef GUIGestureInfo_h__
#define GUIGestureInfo_h__

#include "IGUIPanelElement.h"
#include "GUIGestureElementLayout.h"
#include "GUIElementFactoryDef.h"
#include "ImageSet.h"



namespace mray
{
namespace GUI
{

	enum class EGestureType
	{
		None,
		Swipe,
		Tap,
		Circle,
		Clapping,
		InOut_DoubleSwipe,
		Keytap,
		OpenClose,
		PushPull,
		SteeringWheel,
		FingerCount
	};

class GUIGestureInfo:public IGUIPanelElement, public GUIGestureElementLayout
{

	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
	EGestureType m_gesture;
	video::ImageSetPtr m_imageset;
	float m_alpha;
	float m_time;
public:
	GUIGestureInfo(IGUIManager* m);
	virtual~GUIGestureInfo();

	void SetGesture(EGestureType type);
	EGestureType GetGesture(){ return m_gesture; }

	virtual void Update(float dt);

};
DECLARE_ELEMENT_FACTORY(GUIGestureInfo);

}
}

#endif // GUIGestureInfo_h__
