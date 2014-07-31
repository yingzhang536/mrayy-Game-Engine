


#ifndef GUITransitionBars_h__
#define GUITransitionBars_h__


#include "IGUIElement.h"
#include "GUIElementFactory.h"

namespace mray
{
namespace GUI
{

class GUITransitionBars:public IGUIElement
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
	struct BarInfo
	{
		float width;
		float offset;
	};
	enum EStatus
	{
		EIdle,
		EPlaying
	};
	EStatus m_status;
	float m_speed;
	float m_time;
	float m_angle;
	int m_barsCount;
	video::SColor m_color;
	video::SColor m_targetColor;
	std::vector<BarInfo> m_bars;
public:
	GUITransitionBars(IGUIManager* m);
	virtual ~GUITransitionBars();

	void Start(const video::SColor& targetColor);

	virtual void Draw(const math::rectf*vp);

	virtual void Update(float dt);
	
};
DECLARE_ELEMENT_FACTORY(GUITransitionBars);

}
}

#endif // GUITransitionBars_h__
