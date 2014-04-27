

#ifndef GUITypingTextBox_h__
#define GUITypingTextBox_h__



#include "IGUIElement.h"
#include "GUIElementFactoryDef.h"


namespace mray
{
namespace GUI
{

class GUITypingTextBox :public IGUIElement
{
	DECLARE_RTTI
protected:
	FontAttributes m_fontAttrs;
	float m_charCount;
	float m_typeSpeed;
	int m_minChar;
public:
	static const GUID ElementType;
	DECLARE_PROPERTY_TYPE(TypingSpeed, float, );
	DECLARE_PROPERTY_TYPE(MinChar, int, );
public:
	GUITypingTextBox(IGUIManager* mngr);
	virtual~GUITypingTextBox();

	DECLARE_SETGET_PROP(TypingSpeed, float);
	DECLARE_SETGET_PROP(MinChar, int);

	virtual void Draw(const math::rectf*vp);
	virtual void Update(float dt);


	virtual FontAttributes* GetFontAttributes();
};
DECLARE_ELEMENT_FACTORY(GUITypingTextBox);

}
}

#endif // GUITypingTextBox_h__