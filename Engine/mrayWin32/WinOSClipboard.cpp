#include "stdafx.h"

#include "WinOSClipboard.h"
#include <windows.h>
#include <time.h>
#include "mString.h"

namespace mray{
namespace OS{

	WinOSClipboard::WinOSClipboard()
	{

	}
	WinOSClipboard:: ~WinOSClipboard()
	{

	}
void WinOSClipboard::copyToClipboard(const  mchar*data)
{
	if(!data || !OpenClipboard(0))return;
	EmptyClipboard();

	HGLOBAL clipData;
	 mchar*buffer;

	clipData=GlobalAlloc(GMEM_DDESHARE,sizeof( mchar)*(strLength(data)+1));
	buffer=( mchar*)GlobalLock(clipData);
	
	strCopy(buffer,data);
	
	GlobalUnlock(clipData);
	SetClipboardData(CF_UNICODETEXT,buffer);
	CloseClipboard();

}

 mchar* WinOSClipboard::getClipboardText()
{
	if(!OpenClipboard(0))return 0;

	 mchar*buffer=0;

	HANDLE hData=GetClipboardData(CF_UNICODETEXT);
	buffer=( mchar*)GlobalLock(hData);
	GlobalUnlock(hData);
	CloseClipboard();
	return buffer;


}



}
}
