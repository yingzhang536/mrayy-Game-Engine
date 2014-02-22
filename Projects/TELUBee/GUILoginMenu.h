#include "IGUISchemeBase.h"
#include "GUIButton.h"
#include "GUIEditBox.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUILoginMenu:public GUI::IGUISchemeBase
{

public:
	GUIEditBox* UserTxt;
	GUIEditBox* PasswordTxt;
	GUIButton* StartBtn;
	GUIButton* SeeThroughBtn;
	GUIButton* ExitBtn;
	GUIButton* ConnectRemoteBtn;
	GUIButton* ConnectLocalBtn;
	GUIButton* PlayVideoBtn;
	GUIStaticText* WarningLbl;
	GUIStaticText* MessageLbl;

public:

	GUILoginMenu():UserTxt(0),PasswordTxt(0),StartBtn(0),SeeThroughBtn(0),ExitBtn(0),ConnectRemoteBtn(0),ConnectLocalBtn(0),PlayVideoBtn(0),WarningLbl(0),MessageLbl(0)
	{		
		m_elementsMap["UserTxt"]=(IGUIElement**)&UserTxt;
		m_elementsMap["PasswordTxt"]=(IGUIElement**)&PasswordTxt;
		m_elementsMap["StartBtn"]=(IGUIElement**)&StartBtn;
		m_elementsMap["SeeThroughBtn"]=(IGUIElement**)&SeeThroughBtn;
		m_elementsMap["ExitBtn"]=(IGUIElement**)&ExitBtn;
		m_elementsMap["ConnectRemoteBtn"]=(IGUIElement**)&ConnectRemoteBtn;
		m_elementsMap["ConnectLocalBtn"]=(IGUIElement**)&ConnectLocalBtn;
		m_elementsMap["PlayVideoBtn"]=(IGUIElement**)&PlayVideoBtn;
		m_elementsMap["WarningLbl"]=(IGUIElement**)&WarningLbl;
		m_elementsMap["MessageLbl"]=(IGUIElement**)&MessageLbl;

	}

};
}