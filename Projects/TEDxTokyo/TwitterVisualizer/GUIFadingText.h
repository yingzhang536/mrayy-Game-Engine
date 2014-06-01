

#ifndef GUIFadingText_h__
#define GUIFadingText_h__


#include "IGUIElement.h"
#include "GUIElementFactoryDef.h"
#include "FontAttributes.h"

namespace mray
{
namespace GUI
{

class GUIFadingText :public IGUIElement
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:

	float m_targetAlpha;
	GUI::FontAttributes m_fontAttrs;
	core::UTFString m_targetText;
	float m_time;

public:
	GUIFadingText(GUI::IGUIManager* creator);
	virtual~GUIFadingText();

	virtual bool SetText(const core::UTFString&t);

	void SetTargetAlpha(float a){ m_targetAlpha = a; }

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);

	virtual FontAttributes* GetFontAttributes(){ return &m_fontAttrs; }
};


DECLARE_ELEMENT_FACTORY(GUIFadingText);

}
}

#endif // GUIFadingText_h__
