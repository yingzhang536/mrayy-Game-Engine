#ifndef GUISessionSidePanel_h__
#define GUISessionSidePanel_h__

#include "IGUIElement.h"
#include "GUIElementFactoryDef.h"

namespace mray
{
namespace ted
{
	class SessionContainer;
}
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

	ted::SessionContainer* m_sessions;

public:
	GUISessionSidePanel(IGUIManager* m);
	virtual~GUISessionSidePanel();


	void SetSessionContainer(ted::SessionContainer* s);

	virtual bool OnEvent(Event* e);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
};
DECLARE_ELEMENT_FACTORY(GUISessionSidePanel);

}
}

#endif // GUISessionSidePanel_h__
