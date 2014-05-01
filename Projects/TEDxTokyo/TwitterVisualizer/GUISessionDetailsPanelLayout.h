#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUISessionDetailsPanelLayout:public GUI::IGUISchemeBase
{

public:
	GUIStaticText* SessionName;
	GUIStaticText* SessionTime;

public:

	GUISessionDetailsPanelLayout():SessionName(0),SessionTime(0)
	{		
		m_elementsMap["SessionName"]=(IGUIElement**)&SessionName;
		m_elementsMap["SessionTime"]=(IGUIElement**)&SessionTime;

	}

};
}