#ifndef __GUIAugTelScreenLayout__
#define __GUIAugTelScreenLayout__
#include "IGUISchemeBase.h"
#include "GUINavigationElement.h"
#include "GUIPanel.h"
#include "GUITypingTextBox.h"
namespace mray{

using namespace GUI;
class GUIAugTelScreenLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUINavigationElement* NavElem;
	GUIPanel* WaitPanel;
	GUITypingTextBox* WaitTxt;

public:

	GUIAugTelScreenLayout():Root(0),NavElem(0),WaitPanel(0),WaitTxt(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["NavElem"]=(IGUIElement**)&NavElem;
		m_elementsMap["WaitPanel"]=(IGUIElement**)&WaitPanel;
		m_elementsMap["WaitTxt"]=(IGUIElement**)&WaitTxt;

	}

};
}
#endif
