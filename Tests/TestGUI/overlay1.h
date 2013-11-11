#include "IGUISchemeBase.h"
#include "GUIButton.h"
#include "GUIEditBox.h"
namespace mray{

using namespace GUI;
class overlay1:public GUI::IGUISchemeBase
{

public:
	GUIButton* Button1;
	GUIButton* Button2;
	GUIButton* Button3;
	GUIButton* Button4;
	GUIEditBox* NameTxt;
	GUIEditBox* AgeTxt;
	GUIEditBox* EmailTxt;
	GUIEditBox* DetailsTxt;

public:

	overlay1():Button1(0),Button2(0),Button3(0),Button4(0),NameTxt(0),AgeTxt(0),EmailTxt(0),DetailsTxt(0)
	{		
		m_elementsMap["Button1"]=(IGUIElement**)&Button1;
		m_elementsMap["Button2"]=(IGUIElement**)&Button2;
		m_elementsMap["Button3"]=(IGUIElement**)&Button3;
		m_elementsMap["Button4"]=(IGUIElement**)&Button4;
		m_elementsMap["NameTxt"]=(IGUIElement**)&NameTxt;
		m_elementsMap["AgeTxt"]=(IGUIElement**)&AgeTxt;
		m_elementsMap["EmailTxt"]=(IGUIElement**)&EmailTxt;
		m_elementsMap["DetailsTxt"]=(IGUIElement**)&DetailsTxt;

	}

};
}