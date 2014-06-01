#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStackPanel.h"
#include "GUIStaticImage.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUIGestureElementLayout:public GUI::IGUISchemeBase
{

public:
	GUIStackPanel* Root;
	GUIStaticImage* BackgroundImage;
	GUIStaticImage* GestureImage;
	GUIStaticText* GestureName;

public:

	GUIGestureElementLayout():Root(0),BackgroundImage(0),GestureImage(0),GestureName(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["BackgroundImage"]=(IGUIElement**)&BackgroundImage;
		m_elementsMap["GestureImage"]=(IGUIElement**)&GestureImage;
		m_elementsMap["GestureName"]=(IGUIElement**)&GestureName;

	}

};
}