#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUITweetDetailsPanelLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIStaticText* SessionName;
	GUIStaticText* Description;
	GUIStaticText* SessionTime;

public:

	GUITweetDetailsPanelLayout():Root(0),SessionName(0),Description(0),SessionTime(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["SessionName"]=(IGUIElement**)&SessionName;
		m_elementsMap["Description"]=(IGUIElement**)&Description;
		m_elementsMap["SessionTime"]=(IGUIElement**)&SessionTime;

	}

};
}