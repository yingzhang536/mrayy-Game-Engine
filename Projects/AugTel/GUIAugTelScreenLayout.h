#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUITypingTextBox.h"
namespace mray{

using namespace GUI;
class GUIAugTelScreenLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* WaitPanel;
	GUITypingTextBox* WaitTxt;

public:

	GUIAugTelScreenLayout():WaitPanel(0),WaitTxt(0)
	{		
		m_elementsMap["WaitPanel"]=(IGUIElement**)&WaitPanel;
		m_elementsMap["WaitTxt"]=(IGUIElement**)&WaitTxt;

	}

};
}