#ifndef GUISessionSidePanel_h__
#define GUISessionSidePanel_h__

#include "IGUIElement.h"
#include "GUIElementFactoryDef.h"

namespace mray
{
namespace GUI
{

class GUISessionSidePanel:public IGUIElement
{
	DECLARE_RTTI;
public:
	static const core::string ElementType;
protected:
	enum EState
	{
		Expand,
		Shrink
	};

	EState m_state;

	float m_maxWidth;
	float m_minWidth;

public:
	GUISessionSidePanel(IGUIManager* m);
	virtual~GUISessionSidePanel();

	virtual bool OnEvent(Event* e);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
};
DECLARE_ELEMENT_FACTORY(GUISessionSidePanel);

}
}

#endif // GUISessionSidePanel_h__
