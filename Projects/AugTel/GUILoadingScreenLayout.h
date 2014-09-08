#include "IGUISchemeBase.h"
#include "GUIListBox.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
#include "GUITypingTextBox.h"
namespace mray{

using namespace GUI;
class GUILoadingScreenLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* WaitPanel;
	GUITypingTextBox* WaitTxt;
	GUIStaticText* InfoTxt;
	GUIListBox* Status;

public:

	GUILoadingScreenLayout():WaitPanel(0),WaitTxt(0),InfoTxt(0),Status(0)
	{		
		m_elementsMap["WaitPanel"]=(IGUIElement**)&WaitPanel;
		m_elementsMap["WaitTxt"]=(IGUIElement**)&WaitTxt;
		m_elementsMap["InfoTxt"]=(IGUIElement**)&InfoTxt;
		m_elementsMap["Status"]=(IGUIElement**)&Status;

	}

};
}