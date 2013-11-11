#include "IGUISchemeBase.h"
#include "GUIBlinkingText.h"
#include "GUILoadingElement.h"
#include "GUIPanel.h"
#include "GUIStaticImage.h"
namespace mray{

using namespace GUI;
class GUIOculusDetect:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIPanel* ConnectingFrame;
	GUIBlinkingText* StatusLbl;

public:

	GUIOculusDetect():Root(0),ConnectingFrame(0),StatusLbl(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["ConnectingFrame"]=(IGUIElement**)&ConnectingFrame;
		m_elementsMap["StatusLbl"]=(IGUIElement**)&StatusLbl;

	}

};
}