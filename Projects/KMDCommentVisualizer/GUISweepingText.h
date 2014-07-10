

#ifndef GUISweepingText_h__
#define GUISweepingText_h__


#include "IGUIElement.h"
#include "GUIElementFactoryDef.h"
#include "FontAttributes.h"

namespace mray
{
namespace GUI
{



class GUISweepingText:public IGUIElement
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:

	class PastTextIfo
	{
	public:
		PastTextIfo() :time(0)
		{}
		core::UTFString text;
		float time;
		video::SColor clr;
	};

	std::list<PastTextIfo> m_pastText;

	float m_maxTime;
	GUI::FontAttributes m_fontAttrs;
	float m_time;

public:
	GUISweepingText(GUI::IGUIManager* creator);
	virtual~GUISweepingText();

	virtual bool SetText(const core::UTFString&t);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);

	virtual FontAttributes* GetFontAttributes(){ return &m_fontAttrs; }
};


DECLARE_ELEMENT_FACTORY(GUISweepingText);

}
}

#endif // GUISweepingText_h__
