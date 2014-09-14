

#ifndef GUIConsole_h__
#define GUIConsole_h__

#include "GUIConsoleLayout.h"
#include "GUIElementFactoryDef.h"
#include "IGUIPanelElement.h"

namespace mray
{
namespace GUI
{
	
class GUIConsole :public IGUIPanelElement,public GUIConsoleLayout
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:

	void _OnCommand(IObject* obj, void* data);
	void _OnSelectCommand(IObject* obj, void* data);

public:

	DelegateEvent2<GUIConsole*,const core::string&> OnCommand; //this,const mchar* cmd

	GUIConsole(IGUIManager* m);
	virtual ~GUIConsole();

	virtual bool OnEvent(Event* e);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);

	void AddToHistory(const core::string& text, const video::SColor& color);

};
DECLARE_ELEMENT_FACTORY(GUIConsole);
}
}

#endif // GUIConsole_h__
