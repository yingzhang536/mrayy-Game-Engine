	
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

	class CommandManager;

class ICommand
{
private:
protected:
	CommandManager* m_owner;
	bool m_enabled;
public:
	ICommand(CommandManager*owner) :m_owner(owner), m_enabled(true){}
	virtual~ICommand(){}

	CommandManager* GetOwnerManager(){ return m_owner; }
	void SetOwnerManager(CommandManager* o){ m_owner = o; }

	virtual core::string getCommandName() = 0;

	virtual core::string getCommandInfo() = 0;

	virtual int getCommandMinArgCount()=0;

	virtual int getCommandMaxArgCount()=0;

	virtual bool onCommand(std::vector<core::string>& args)=0;

	virtual const core::string& getLastMessage(){ return core::string::Empty; }

	virtual bool isEnabled(){ return m_enabled; }
	virtual void enable(bool e){ m_enabled = e; }
};

}


#endif //___ICommand___
