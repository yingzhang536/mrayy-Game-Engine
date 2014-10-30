

#ifndef GUIInterfaceScreenImpl_h__
#define GUIInterfaceScreenImpl_h__


#include "GUIInterfaceScreen.h"
#include "ImageSet.h"


namespace mray
{
namespace GUI
{

class GUIInterfaceScreenImpl :public IGUIPanelElement,public GUIInterfaceScreen
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
	video::ImageSet* m_imageset;
public:
	GUIInterfaceScreenImpl(IGUIManager* m);
	virtual ~GUIInterfaceScreenImpl();

	void EnableHand(bool e);

	void EnableVideo(bool e);

	void EnableMic(bool e);

	virtual void DoneLoadingElements();
};

}
}

#endif // GUIInterfaceScreenImpl_h__
