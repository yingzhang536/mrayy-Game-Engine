

#ifndef GUIAugTelScreen_h__
#define GUIAugTelScreen_h__


#include "IGUIPanelElement.h"
#include "GUIAugTelScreenLayout.h"

namespace mray
{
namespace GUI
{
	
class GUIAugTelScreen:public IGUIPanelElement,public GUIAugTelScreenLayout
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
public:
	GUIAugTelScreen(IGUIManager* m);
	virtual ~GUIAugTelScreen();

	void OnDisconnected();
	void OnConnected();
	
};

}
}

#endif // GUIAugTelScreen_h__
