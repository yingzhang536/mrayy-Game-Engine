#ifndef __GUIScenarioLayout__
#define __GUIScenarioLayout__
#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticImage.h"
namespace mray{

using namespace GUI;
class GUIScenarioLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIStaticImage* ScenarioImg;

public:

	GUIScenarioLayout():Root(0),ScenarioImg(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["ScenarioImg"]=(IGUIElement**)&ScenarioImg;

	}

};
}
#endif
