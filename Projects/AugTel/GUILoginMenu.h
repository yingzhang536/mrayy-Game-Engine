#ifndef __GUILoginMenu__
#define __GUILoginMenu__
#include "IGUISchemeBase.h"
#include "GUIButton.h"
#include "GUIEditBox.h"
#include "GUIListBox.h"
#include "GUIPanel.h"
#include "GUIStackPanel.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUILoginMenu:public GUI::IGUISchemeBase
{

public:
	GUIListBox* RobotLst;
	GUIEditBox* UserTxt;
	GUIEditBox* PasswordTxt;
	GUIButton* StartBtn;
	GUIButton* SeeThroughBtn;
	GUIButton* ConnectRemoteBtn;
	GUIButton* ConnectLocalBtn;
	GUIButton* DepthViewerBtn;
	GUIButton* ExitBtn;
	GUIStaticText* WarningLbl;
	GUIStaticText* MessageLbl;

public:

	GUILoginMenu():RobotLst(0),UserTxt(0),PasswordTxt(0),StartBtn(0),SeeThroughBtn(0),ConnectRemoteBtn(0),ConnectLocalBtn(0),DepthViewerBtn(0),ExitBtn(0),WarningLbl(0),MessageLbl(0)
	{		
		m_elementsMap["RobotLst"]=(IGUIElement**)&RobotLst;
		m_elementsMap["UserTxt"]=(IGUIElement**)&UserTxt;
		m_elementsMap["PasswordTxt"]=(IGUIElement**)&PasswordTxt;
		m_elementsMap["StartBtn"]=(IGUIElement**)&StartBtn;
		m_elementsMap["SeeThroughBtn"]=(IGUIElement**)&SeeThroughBtn;
		m_elementsMap["ConnectRemoteBtn"]=(IGUIElement**)&ConnectRemoteBtn;
		m_elementsMap["ConnectLocalBtn"]=(IGUIElement**)&ConnectLocalBtn;
		m_elementsMap["DepthViewerBtn"]=(IGUIElement**)&DepthViewerBtn;
		m_elementsMap["ExitBtn"]=(IGUIElement**)&ExitBtn;
		m_elementsMap["WarningLbl"]=(IGUIElement**)&WarningLbl;
		m_elementsMap["MessageLbl"]=(IGUIElement**)&MessageLbl;

	}

};
}
#endif
