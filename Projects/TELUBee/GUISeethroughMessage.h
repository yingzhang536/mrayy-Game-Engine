#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUISeeThroughMessage:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;

public:

	GUISeeThroughMessage():Root(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;

	}

};
}