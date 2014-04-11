

#ifndef ___OS___
#define ___OS___

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <IOSClipboard.h>
#include "compileConfig.h"

namespace mray{
namespace OS{

//! Operation System methods
class MRAY_Win32_DLL WinOSClipboard:public IOSClipboard
{
public:
	WinOSClipboard();
	virtual ~WinOSClipboard();
	//! copy text to clipboard
	virtual void copyToClipboard(const  mchar*data);
	//! get clipboard text
	virtual  mchar* getClipboardText();
};


};//OS
};//mray


#endif



