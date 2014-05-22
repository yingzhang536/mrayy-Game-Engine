#include "IGUISchemeBase.h"
#include "Button.h"
#include "Panel.h"
namespace mray{

using namespace GUI;
class GUIMainMenu:public GUI::IGUISchemeBase
{

public:
	Button* StartBtn;
	Button* OptionsBtn;
	Button* ExitBtn;

public:

	GUIMainMenu():StartBtn(0),OptionsBtn(0),ExitBtn(0)
	{		
		m_elementsMap["StartBtn"]=(IGUIElement**)&StartBtn;
		m_elementsMap["OptionsBtn"]=(IGUIElement**)&OptionsBtn;
		m_elementsMap["ExitBtn"]=(IGUIElement**)&ExitBtn;

	}

};
}