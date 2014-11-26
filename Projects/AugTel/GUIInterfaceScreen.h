#ifndef __GUIInterfaceScreen__
#define __GUIInterfaceScreen__
#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticImage.h"
namespace mray{

using namespace GUI;
class GUIInterfaceScreen:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Icons;
	GUIStaticImage* HandIcon;
	GUIStaticImage* VideoIcon;
	GUIStaticImage* MicIcon;

public:

	GUIInterfaceScreen():Icons(0),HandIcon(0),VideoIcon(0),MicIcon(0)
	{		
		m_elementsMap["Icons"]=(IGUIElement**)&Icons;
		m_elementsMap["HandIcon"]=(IGUIElement**)&HandIcon;
		m_elementsMap["VideoIcon"]=(IGUIElement**)&VideoIcon;
		m_elementsMap["MicIcon"]=(IGUIElement**)&MicIcon;

	}

};
}
#endif
