#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUISessionDetailsPanelLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIPanel* Contents;
	GUIStaticText* SessionName;
	GUIStaticText* SpeakerID;
	GUIStaticText* SessionTime;

public:

	GUISessionDetailsPanelLayout():Root(0),Contents(0),SessionName(0),SpeakerID(0),SessionTime(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["Contents"]=(IGUIElement**)&Contents;
		m_elementsMap["SessionName"]=(IGUIElement**)&SessionName;
		m_elementsMap["SpeakerID"]=(IGUIElement**)&SpeakerID;
		m_elementsMap["SessionTime"]=(IGUIElement**)&SessionTime;

	}

};
}