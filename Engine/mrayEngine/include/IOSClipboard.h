
/********************************************************************
	created:	2009/01/02
	created:	2:1:2009   16:39
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\IOSClipboard.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	IOSClipboard
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___IOSClipboard___
#define ___IOSClipboard___

#include "ISingleton.h"
#include "mString.h"

namespace mray{
namespace OS{

class IOSClipboard:public ISingleton<IOSClipboard>
{
public:
	IOSClipboard(){}
	virtual~IOSClipboard(){}
	//! copy text to clipboard
	virtual void copyToClipboard(const  mchar*data)=0;
	//! get clipboard text
	virtual mchar* getClipboardText()=0;
};
#define gOSClipboard OS::IOSClipboard::getInstance()

}
}


#endif //___IOSClipboard___

