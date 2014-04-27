#include "IGUISchemeBase.h"
#include "GUIListBox.h"
#include "GUILoadingElement.h"
#include "GUIPanel.h"
#include "GUIStackPanel.h"
#include "GUITypingTextBox.h"
namespace mray{

using namespace GUI;
class GUIConnectingScreen:public GUI::IGUISchemeBase
{

public:
	GUIListBox* StatusLst;
	GUITypingTextBox* CurrentTxt;

public:

	GUIConnectingScreen():StatusLst(0),CurrentTxt(0)
	{		
		m_elementsMap["StatusLst"]=(IGUIElement**)&StatusLst;
		m_elementsMap["CurrentTxt"]=(IGUIElement**)&CurrentTxt;

	}

};
}