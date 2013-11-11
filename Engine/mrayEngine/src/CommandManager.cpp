#include "stdafx.h"


#include "CommandManager.h"


#include "StringUtil.h"

namespace mray{

	class CPrintCommands:public ICommand
	{
		bool m_enabled;
		core::string m_msg;
	public:
		CPrintCommands(){
			m_enabled=true;
		}

		virtual const mchar* getCommandName(){
			return mT("PrintCmd");
		}

		virtual const mchar* getCommandInfo(){
			return mT("List the Avaliable Commands");
		}

		virtual int getCommandMinArgCount(){return 0;}

		virtual int getCommandMaxArgCount(){return 0;}

		virtual bool onCommand(std::vector<core::string>& args)
		{
			CommandList::const_iterator p=CommandManager::getInstance().getCommands().begin();
			CommandList::const_iterator end=CommandManager::getInstance().getCommands().end();
			m_msg=mT("");
			for(;p!=end;++p)
			{
				if(p->second->isEnabled()){
					m_msg+=p->second->getCommandName();
					m_msg+=mT(":");
					m_msg+=p->second->getCommandInfo();
					m_msg+=mT("\n");
				}
			}
			return true;
		}

		virtual const mchar* getLastMessage(){return m_msg.c_str();}

		virtual bool isEnabled(){
			return m_enabled;
		}
		virtual void enable(bool e){
			m_enabled=e;
		}
	};

CommandManager::CommandManager(){
	addCommand(new CPrintCommands());
}
CommandManager::~CommandManager(){
}

void CommandManager::addCommand(GCPtr<ICommand> cmd){
	core::string s=core::StringUtil::ToLower(cmd->getCommandName());
	m_commands[s]=cmd;
}

bool CommandManager::execCommand(const core::string&cmd){
	m_args=core::StringUtil::Split(cmd,mT(" "));

	if(m_args.size()==0){
		m_lastMsg=mT("Enter Command Name!");
		return false;
	}
	core::string s=core::StringUtil::ToLower(m_args[0]);
	CommandList::iterator it= m_commands.find(s);
	bool res=false;
	if(it!=m_commands.end()){
		GCPtr<ICommand> cmd=it->second;
		if(m_args.size()-1<cmd->getCommandMinArgCount() ||
			m_args.size()-1>cmd->getCommandMaxArgCount() )
		{
			m_lastMsg=mT("Parameters count not matched!");
			return false;
		}
		res= cmd->onCommand(m_args);
		m_lastMsg=cmd->getLastMessage();
	}else{
		m_lastMsg=mT("Command Not Found!");
	}
	return res;
}

void CommandManager::removeCommand(GCPtr<ICommand> cmd){
	CommandList::iterator it= m_commands.begin();
	for (;it!=m_commands.end();++it)
	{
		if(it->second==cmd){
			m_commands.erase(it);
			return;
		}
	}
}
void CommandManager::removeCommand(const core::string&name){
	m_commands.erase(name);
}

const  core::string&CommandManager::getLastMessage(){
	return m_lastMsg;
}

const CommandList& CommandManager::getCommands(){
	return m_commands;
}

}
