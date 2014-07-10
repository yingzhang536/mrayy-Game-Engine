#include "IGUISchemeBase.h"
#include "GUIFadingText.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
#include "GUISweepingText.h"
namespace mray{

using namespace GUI;
class GUIScenePanelLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Background;
	GUISweepingText* Theme;
	GUISweepingText* Speaker;
	GUIFadingText* Info;

public:

	GUIScenePanelLayout():Background(0),Theme(0),Speaker(0),Info(0)
	{		
		m_elementsMap["Background"]=(IGUIElement**)&Background;
		m_elementsMap["Theme"]=(IGUIElement**)&Theme;
		m_elementsMap["Speaker"]=(IGUIElement**)&Speaker;
		m_elementsMap["Info"]=(IGUIElement**)&Info;

	}

};
}