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
	void WinOSClipboard::copyToClipboard(const  core::UTFString& data)
{
	if( !OpenClipboard(0))return;
	EmptyClipboard();

	HGLOBAL clipData;
	utf32*buffer;

	 clipData = GlobalAlloc(GMEM_DDESHARE, sizeof(utf32)*data.Length());
	 buffer = (utf32*)GlobalLock(clipData);
	
	 memcpy(buffer, data.c_str(), sizeof(utf32)*data.Length());
	//strCopy(buffer,data);
	
	GlobalUnlock(clipData);
	SetClipboardData(CF_UNICODETEXT,buffer);
	CloseClipboard();

}

core::UTFString WinOSClipboard::getClipboardText()
{
	if(!OpenClipboard(0))
		return core::UTFString::Empty;


	HANDLE hData=GetClipboardData(CF_UNICODETEXT);
	
	wchar_t* buffer=( wchar_t*)GlobalLock(hData);
	GlobalUnlock(hData);
	CloseClipboard();
	return core::UTFString(buffer);


}



}
}
