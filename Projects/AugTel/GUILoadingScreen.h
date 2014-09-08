
#ifndef GUILoadingScreen_h__
#define GUILoadingScreen_h__




#include "IGUIPanelElement.h"
#include "GUILoadingScreenLayout.h"

namespace mray
{
namespace GUI
{

class GUILoadingScreen :public IGUIPanelElement, public GUILoadingScreenLayout
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
public:
	GUILoadingScreen(IGUIManager* m);
	virtual ~GUILoadingScreen();

	void Reset();
	void AddStatus(const core::string& st);

	void SetText(const core::string& text, bool waitText);
	void SetInfo(const core::string& st);
};

}
}
#endif // GUILoadingScreen_h__

