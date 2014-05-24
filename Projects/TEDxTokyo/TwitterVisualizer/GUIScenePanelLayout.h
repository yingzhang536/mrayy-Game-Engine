#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUISweepingText.h"
namespace mray{

using namespace GUI;
class GUIScenePanelLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Background;
	GUISweepingText* Theme;

public:

	GUIScenePanelLayout():Background(0),Theme(0)
	{		
		m_elementsMap["Background"]=(IGUIElement**)&Background;
		m_elementsMap["Theme"]=(IGUIElement**)&Theme;

	}

};
}