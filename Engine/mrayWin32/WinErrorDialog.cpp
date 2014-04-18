#include "stdafx.h"

#include "resource.h"
#include "WinErrorDialog.h"

namespace mray{


BOOL CALLBACK WinErrorDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	core::string str;
	HWND dlgItem ;
	WinErrorDialog*dialog=dynamic_cast<WinErrorDialog*>(IErrorDialog::getInstancePtr());
	switch(iMsg){
		case WM_PAINT:
		{
			PAINTSTRUCT p;
			BeginPaint(hDlg,&p);
			EndPaint(hDlg,&p);
		}break;
		case WM_INITDIALOG:
		{

			dlgItem=GetDlgItem(hDlg,IDC_ERROR_MSG);
			
			SetWindowText(dlgItem,dialog->m_errorMsg.c_str());

            int x, y, screenWidth, screenHeight;
            RECT rcDlg;
            GetWindowRect(hDlg, &rcDlg);
            screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
            screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

            x = (screenWidth / 2) - ((rcDlg.right - rcDlg.left) / 2);
            y = (screenHeight / 2) - ((rcDlg.bottom - rcDlg.top) / 2);

            MoveWindow(hDlg, x, y, (rcDlg.right - rcDlg.left),
                (rcDlg.bottom - rcDlg.top), TRUE);

			return true;
		}
		
        case WM_COMMAND:
		{
            switch (LOWORD(wParam))
			{
				case IDCLOSE:
				{
					EndDialog(hDlg,1);
					return true;
				}
				case IDOK:
				{
					EndDialog(hDlg,1);
					return true;
				}

			}break;
		}break;


	}
	return false;
}

WinErrorDialog::WinErrorDialog(){
#ifndef MRAY_WinApp_DLL_EXPORT
	m_hInstance=GetModuleHandle(0);
#else 
#	ifdef _DEBUG
	m_hInstance=GetModuleHandle(mT("mrayWinApp_d.dll"));
#	else
	m_hInstance=GetModuleHandle(mT("mrayWinApp.dll"));
#	endif
#endif
}
WinErrorDialog::~WinErrorDialog(){
}

bool WinErrorDialog::run(const core::string&msg){
	m_errorMsg=msg;
	DialogBox(m_hInstance,MAKEINTRESOURCE(IDD_DIALOG_ERROR),0,DlgProc);

	return true;
}


}

