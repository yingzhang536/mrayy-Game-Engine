
/********************************************************************
	created:	2009/05/04
	created:	4:5:2009   20:06
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\ICommand.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	ICommand
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___ICommand___
#define ___ICommand___


#include "mstring.h"

namespace mray{

class ICommand
{
private:
protected:
public:
	ICommand(){}
	virtual~ICommand(){}

	virtual const mchar* getCommandName()=0;

	virtual const mchar* getCommandInfo()=0;

	virtual int getCommandMinArgCount()=0;

	virtual int getCommandMaxArgCount()=0;

	virtual bool onCommand(std::vector<core::string>& args)=0;

	virtual const mchar* getLastMessage()=0;

	virtual bool isEnabled()=0;
	virtual void enable(bool e)=0;
};

}


#endif //___ICommand___
