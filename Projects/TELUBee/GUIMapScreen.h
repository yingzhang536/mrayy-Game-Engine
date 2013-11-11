#include "IGUISchemeBase.h"
#include "GUIBlinkingText.h"
#include "GUILoadingElement.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUIMapScreen:public GUI::IGUISchemeBase
{

public:
	GUIStaticText* UserNameLbl;
	GUIStaticText* DateTime;

public:

	GUIMapScreen():UserNameLbl(0),DateTime(0)
	{		
		m_elementsMap["UserNameLbl"]=(IGUIElement**)&UserNameLbl;
		m_elementsMap["DateTime"]=(IGUIElement**)&DateTime;

	}

};
}